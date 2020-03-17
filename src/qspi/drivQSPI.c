
#include "qspi/drivQSPI.h"
#include "FreeRTOS.h"
#include "qspi/MT25Q.h"
#include "task.h"
#include "queue.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_dma.h"
#include "priorities.h"
#include "rand.h"
//#include "gui.h"
#include <__cross_studio_io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stm32f769xx.h>

#define JEDEC_MAN_ID    0x20
#define MICRON_3V_TYPE  0xBA
#define MICRON_512Mb    0x20
#define DMA_THRESHOLD   32

#define QSPI_SR_TC_FLAG     (0x02)
#define QSPI_SR_BUSY_FLAG   (0x20)
#define QSPI_FLASH_OFFSET   (0x90000000)
#define QSPI_FLASH_SIZE     (0x4000000)//64MB
#define QSPI_FLASH_READONLY (0x0E00000)//0-0x7FFFFF 8MB

#define QSPI_CCR_1LINE      (0x01)
#define QSPI_CCR_4LINES     (0x03)
#define QSPI_CCR_ADRx4      (0x03)
#define QSPI_CCR_INSTR(x)   (x & 0b11111111)
#define QSPI_CCR_IMODE(x)   (x<<8)
#define QSPI_CCR_ADMODE(x)  (x<<10)
#define QSPI_CCR_ADSIZE(x)  (x<<12)
#define QSPI_CCR_ABMODE(x)  (x<<14)
#define QSPI_CCR_ABSIZE(x)  (x<<16)
#define QSPI_CCR_DCYC(x)    (x<<18)
#define QSPI_CCR_DMODE(x)   (x<<24)
#define QSPI_CCR_FMODE_AP   (2UL<<26)
#define QSPI_CCR_FMODE_IR   (1UL<<26)
#define QSPI_CCR_FMODE_IW   (0UL<<26)
#define QSPI_CCR_SIOO       (1UL<<28)
#define QSPI_CCR_DHHC       (1UL<<30)
#define QSPI_CCR_DDRM       (1UL<<31)
#define QSPI_DLR_COUNT(x)   (x-1)
#define QSPI_PAGEP_POLL_INTERVAL  (1000) //500 = 13 us at 35mhz
#define QSPI_ERASE_POLL_INTERVAL  (5000) //500 = 13 us at 35mhz

#define QSPI_ACTION_NONE  0
#define QSPI_ACTION_READ  1
#define QSPI_ACTION_WRITE 2
#define QSPI_ACTION_ERASE 3

#define QSPI_MAX_CMD_QUEUE 8

typedef struct {
  TaskHandle_t xCallingTask;
  uint32_t command;
  uint8_t * flashAddress;
  uint8_t *data;
  uint32_t length;
  uint32_t * ret;
} QSPI_CMD;

static volatile uint32_t qspiAction = QSPI_ACTION_NONE;
static volatile uint32_t drivStatus = DRIV_QSPI_STATUS_INIT;

TaskHandle_t xqspiTaskHandle = NULL;
static QueueHandle_t xQueueCmd = NULL;
static void qspiTasks(void *pvParameters);
static uint32_t readFlash(uint8_t *flashAddress, uint8_t *data, uint16_t length);
static uint32_t writeFlashPage(uint8_t *flashAddress, uint8_t *data, uint32_t length);
static uint32_t eraseSector(uint8_t *flashAddress, uint32_t sectorSize);

/* Public */
void drivQSPIinit(void) {
  xTaskCreate(qspiTasks, "qspiTasks", 250, NULL, PRIORITY_RTOS_QSPI, &xqspiTaskHandle);
  xQueueCmd = xQueueCreate(QSPI_MAX_CMD_QUEUE, sizeof(QSPI_CMD));
  configASSERT(xQueueCmd != NULL);
}

uint32_t qspiReadFlash(uint8_t *flashAddress, uint8_t *data, uint32_t length) {
  uint32_t ret = 0;
  uint32_t rlen;
  QSPI_CMD cmd;
  if ((uint32_t)flashAddress >= QSPI_FLASH_OFFSET) {
    flashAddress -= QSPI_FLASH_OFFSET;
  }
  if ((uint32_t)flashAddress + length > QSPI_FLASH_SIZE) {
    return DRIV_QSPI_ERR_BAD_PARAMS;
  } else if (data == NULL) {
    return DRIV_QSPI_ERR_NULL_PTR;
  }
  while (length) {
    rlen = length > 0xFF00 ? 0xFF00 : length;
    cmd.xCallingTask = xTaskGetCurrentTaskHandle();
    cmd.command = QSPI_ACTION_READ;
    cmd.flashAddress = flashAddress;
    cmd.data = data;
    cmd.length = rlen;
    cmd.ret = &ret;
    xQueueSendToFront(xQueueCmd, (void *)&cmd, portMAX_DELAY);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (ret) {
      return ret;
    }
    data += rlen;
    flashAddress += rlen;
    length -= rlen;
  }
  return ret;
}

uint32_t qspiWriteFlash(uint8_t *flashAddress, uint8_t *data, uint32_t length) {
  uint32_t ret = 0;
  QSPI_CMD cmd;
  if ((uint32_t)flashAddress >= QSPI_FLASH_OFFSET) {
    flashAddress -= QSPI_FLASH_OFFSET;
  }
  if ((uint32_t)flashAddress > QSPI_FLASH_READONLY) {
    return DRIV_QSPI_ERR_READONLY;
  } else if ((uint32_t)flashAddress % 0x100) {
    return DRIV_QSPI_ERR_PAGE_ADDR;
  }
  cmd.xCallingTask = xTaskGetCurrentTaskHandle();
  cmd.command = QSPI_ACTION_WRITE;
  cmd.flashAddress = flashAddress;
  cmd.data = data;
  cmd.length = length;
  cmd.ret = &ret;
  xQueueSendToBack(xQueueCmd, (void *)&cmd, portMAX_DELAY);
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  return ret;
}

uint32_t qspiEraseSector(uint8_t *flashAddress, uint32_t sectorLength) {
  uint32_t ret = 0;
  QSPI_CMD cmd;
  if ((uint32_t)flashAddress >= QSPI_FLASH_OFFSET) {
    flashAddress -= QSPI_FLASH_OFFSET;
  }
  if ((uint32_t)flashAddress > QSPI_FLASH_READONLY) {
    return DRIV_QSPI_ERR_READONLY;
  }
  cmd.xCallingTask = xTaskGetCurrentTaskHandle();
  cmd.command = QSPI_ACTION_ERASE;
  cmd.flashAddress = flashAddress;
  cmd.data = NULL;
  cmd.length = sectorLength;
  cmd.ret = &ret;
  xQueueSendToBack(xQueueCmd, (void *)&cmd, portMAX_DELAY);
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  return ret;
}

uint32_t qspiDriverStatus(void) {
  return drivStatus;
}

/* Private */
static uint32_t drvInit(void) {
  uint32_t i = 0;
  struct {
    uint8_t manId;
    uint8_t memType;
    uint8_t memCapacity;
  } id;
  NVIC_SetPriority(DMA2_Stream2_IRQn, PRIORITY_QUADSPI_DMA);
  NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  NVIC_SetPriority(QUADSPI_IRQn, PRIORITY_QUADSPI_INT);
  NVIC_EnableIRQ(QUADSPI_IRQn);
  //Reset QUADIO if is quad IO currently
  QUADSPI->CCR =
      QSPI_CCR_INSTR(SPI_FLASH_RST_QUAD) |
      QSPI_CCR_IMODE(QSPI_CCR_4LINES) |
      QSPI_CCR_FMODE_IW;

  while (QUADSPI->SR & QSPI_SR_BUSY_FLAG) {
  };
  QUADSPI->DLR = QSPI_DLR_COUNT(3);
  QUADSPI->CCR =
      QSPI_CCR_INSTR(SPI_FLASH_INS_RDID) |
      QSPI_CCR_IMODE(QSPI_CCR_1LINE) |
      QSPI_CCR_DCYC(0) |
      QSPI_CCR_DMODE(QSPI_CCR_1LINE) |
      QSPI_CCR_FMODE_IR;
  QUADSPI->AR = 0;
  while (!(QUADSPI->SR & QSPI_SR_TC_FLAG)) { //tc wait
  };
  id.manId = *(volatile uint8_t *)&QUADSPI->DR;
  id.memType = *(volatile uint8_t *)&QUADSPI->DR;
  id.memCapacity = *(volatile uint8_t *)&QUADSPI->DR;

     //Set to 4 BYTE address
  QUADSPI->CCR =
      QSPI_CCR_INSTR(SPI_FLASH_INS_EN4BYTEADDR) |
      QSPI_CCR_IMODE(QSPI_CCR_1LINE) |
      QSPI_CCR_FMODE_IW;

  while (QUADSPI->SR & QSPI_SR_BUSY_FLAG) {
  }; 
  
  //Set to QUADIO
  QUADSPI->CCR =
      QSPI_CCR_INSTR(SPI_FLASH_ENT_QUAD) |
      QSPI_CCR_IMODE(QSPI_CCR_1LINE) |
      QSPI_CCR_FMODE_IW;

  while (QUADSPI->SR & QSPI_SR_BUSY_FLAG) {
  };

  LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_2);
  LL_DMA_EnableIT_TE(DMA2, LL_DMA_STREAM_2);
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_2, LL_DMA_CHANNEL_11);

  return id.manId == JEDEC_MAN_ID &&
         id.memType == MICRON_3V_TYPE &&
         id.memCapacity == MICRON_512Mb;
}

static uint32_t readFlash(uint8_t *flashAddress, uint8_t *data, uint16_t length) {
  int i;
  qspiAction = QSPI_ACTION_READ;
  if ((uint32_t)flashAddress >= QSPI_FLASH_OFFSET) {
    flashAddress -= QSPI_FLASH_OFFSET;
  }
  QUADSPI->CR &= ~(QUADSPI_CR_TCIE); //Disable Transfer complete flag
  QUADSPI->FCR = QUADSPI_FCR_CTCF;//Clear TCIF
  if (length <= DMA_THRESHOLD) {
    QUADSPI->CR &= ~(QUADSPI_CR_DMAEN);
    QUADSPI->FCR = QUADSPI_FCR_CTCF;
    QUADSPI->DLR = QSPI_DLR_COUNT(length);

    QUADSPI->CCR =
        QSPI_CCR_INSTR(SPI_FLASH_INS_FAST_READ) |
        QSPI_CCR_IMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_DCYC(10) |
        QSPI_CCR_DMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_ADSIZE(QSPI_CCR_ADRx4) |
        QSPI_CCR_ADMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_FMODE_IR;
    QUADSPI->AR = (uint32_t)flashAddress;
    while (!(QUADSPI->SR & QSPI_SR_TC_FLAG)) { //tc wait
    };
    for (i = 0; i < length; i++) {
      *data++ = *(uint8_t *)&QUADSPI->DR;
    }
    return 0;
  } else {

    //Set up dma
    LL_DMA_ConfigTransfer(DMA2,
        LL_DMA_STREAM_2,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_PRIORITY_MEDIUM | LL_DMA_MODE_NORMAL |
            LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
            LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_ConfigAddresses(DMA2,
        LL_DMA_STREAM_2,
        (uint32_t)&QUADSPI->DR, (uint32_t)data,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, length);

    //Fire it off ?
    LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);

    QUADSPI->FCR = QUADSPI_FCR_CTCF;
    QUADSPI->DLR = QSPI_DLR_COUNT(length);

    QUADSPI->CCR =
        QSPI_CCR_INSTR(SPI_FLASH_INS_FAST_READ) |
        QSPI_CCR_IMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_DCYC(10) |
        QSPI_CCR_DMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_ADSIZE(QSPI_CCR_ADRx4) |
        QSPI_CCR_ADMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_FMODE_IR;
    QUADSPI->AR = (uint32_t)flashAddress;
    QUADSPI->CR |= QUADSPI_CR_DMAEN;

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    return 0;
  }
}

static void writeEnable(void) {
  //Write enable
  QUADSPI->CCR =
      QSPI_CCR_INSTR(SPI_FLASH_INS_WREN) |
      QSPI_CCR_IMODE(QSPI_CCR_4LINES) |
      QSPI_CCR_FMODE_IW;

  while (QUADSPI->SR & QSPI_SR_BUSY_FLAG)
    ;
  QUADSPI->FCR = QUADSPI_FCR_CTCF; //Clear TCIF
}

static uint32_t writeFlashPage(uint8_t *flashAddress, uint8_t *data, uint32_t length) {
  int i;
  if (!length) {
    return 0;
  }
  qspiAction = QSPI_ACTION_WRITE;
  if ((uint32_t)flashAddress >= QSPI_FLASH_OFFSET) {
    flashAddress -= QSPI_FLASH_OFFSET;
  }
  QUADSPI->CR &= ~(QUADSPI_CR_TCIE); //Disable Transfer complete interrupt
  QUADSPI->FCR = QUADSPI_FCR_CTCF;   //Clear TCIF
  //Write enable
  writeEnable();

  if (length <= DMA_THRESHOLD) {
    QUADSPI->CR &= ~(QUADSPI_CR_DMAEN);
    QUADSPI->DLR = QSPI_DLR_COUNT(length);

    QUADSPI->CCR =
        QSPI_CCR_INSTR(SPI_FLASH_INS_QIFP) |
        QSPI_CCR_IMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_DCYC(0) |
        QSPI_CCR_DMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_ADSIZE(QSPI_CCR_ADRx4) |
        QSPI_CCR_ADMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_FMODE_IW;
    QUADSPI->AR = (uint32_t)flashAddress;

    for (i = 0; i < length; i++) {
      *(uint8_t *)&QUADSPI->DR = *data++;
    }
    while (!(QUADSPI->SR & QSPI_SR_TC_FLAG)) { //tc wait
    };
    return 0;
  } else {

    //Set up dma
    LL_DMA_ConfigTransfer(DMA2,
        LL_DMA_STREAM_2,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_MEDIUM | LL_DMA_MODE_NORMAL |
            LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
            LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE);

    LL_DMA_ConfigAddresses(DMA2,
        LL_DMA_STREAM_2,
        (uint32_t)data, (uint32_t)&QUADSPI->DR,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, length);

    //Fire it off ?

    LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);

    QUADSPI->FCR = QUADSPI_FCR_CTCF;
    QUADSPI->DLR = QSPI_DLR_COUNT(length);

    QUADSPI->CCR =
        QSPI_CCR_INSTR(SPI_FLASH_INS_QIFP) |
        QSPI_CCR_IMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_DCYC(0) |
        QSPI_CCR_DMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_ADSIZE(QSPI_CCR_ADRx4) |
        QSPI_CCR_ADMODE(QSPI_CCR_4LINES) |
        QSPI_CCR_FMODE_IW;


    #if 1
    (void)QUADSPI->CCR;
    QUADSPI->AR = (uint32_t)flashAddress;
    QUADSPI->CR |= (QUADSPI_CR_DMAEN | QUADSPI_CR_TCIE);
    #else //bad
    QUADSPI->CR |= (QUADSPI_CR_DMAEN | QUADSPI_CR_TCIE);
    __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
    QUADSPI->AR = (uint32_t)flashAddress;
    #endif

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    return 0;
  }
}

static uint32_t eraseSector(uint8_t *flashAddress, uint32_t sectorSize) {
  int i;
  qspiAction = QSPI_ACTION_ERASE;
  uint8_t command;
  switch (sectorSize) {
  case DRIV_QSPI_ERASE_4K:
    command = SPI_FLASH_INS_SSE;
    break;
  case DRIV_QSPI_ERASE_32K:
    command = SPI_FLASH_INS_SSE32K;
    break;
  case DRIV_QSPI_ERASE_64K:
    command = SPI_FLASH_INS_SE;
    break;
  default:
    return DRIV_QSPI_ERR_INVALID_SIZE;
  }
  if ((uint32_t)flashAddress >= QSPI_FLASH_OFFSET) {
    flashAddress -= QSPI_FLASH_OFFSET;
  }
  QUADSPI->CR &= ~(QUADSPI_CR_TCIE); //Disable Transfer complete interrupt
  QUADSPI->FCR = QUADSPI_FCR_CTCF;   //Clear TCIF
  //Write enable
  writeEnable();

  QUADSPI->CR &= ~(QUADSPI_CR_DMAEN);
  QUADSPI->DLR = 0;
  
  QUADSPI->CCR =
      QSPI_CCR_INSTR(command) |
      QSPI_CCR_IMODE(QSPI_CCR_4LINES) |
      QSPI_CCR_ADSIZE(QSPI_CCR_ADRx4) |
      QSPI_CCR_ADMODE(QSPI_CCR_4LINES) |
      QSPI_CCR_FMODE_IW;
  QUADSPI->AR = (uint32_t)flashAddress;
  QUADSPI->CR|= QUADSPI_CR_TCIE; //Enable Transfer complete interrupt
  //Interrupt will enable auto poll for finish
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  return 0;
}

static void processRead(QSPI_CMD *cmd) {
  *cmd->ret = readFlash(cmd->flashAddress, cmd->data, cmd->length);
 // debug_printf("processRead 0x%X %i\n",(uint32_t)cmd->flashAddress,cmd->length);
}

/* private processWrite
 * returns finish state
 */
static uint32_t processWrite(QSPI_CMD *cmd) {

  uint32_t programLength = 0;
  if (cmd->length > 0x100) {
    programLength = 0x100;
  } else {
    programLength = cmd->length;
  }
  *cmd->ret = writeFlashPage(cmd->flashAddress, cmd->data, programLength);
  cmd->length -= programLength;
  cmd->flashAddress += programLength;
  cmd->data += programLength;
  //debug_printf("processWrite 0x%X %i\n",(uint32_t)cmd->flashAddress,cmd->length);
  return (cmd->length > 0);
}

/* private processErase
 * returns finish state
 */
static uint32_t processErase(QSPI_CMD *cmd) {
  switch (cmd->length) {
  case DRIV_QSPI_ERASE_4K:
    if ((uint32_t)cmd->flashAddress % 0x1000) {
      *cmd->ret = DRIV_QSPI_ERR_INVALID_START_ADDR;
    } else {
      *cmd->ret = eraseSector(cmd->flashAddress, DRIV_QSPI_ERASE_4K);
    }
    break;
  case DRIV_QSPI_ERASE_32K:
    if ((uint32_t)cmd->flashAddress % 0x8000) {
      *cmd->ret = DRIV_QSPI_ERR_INVALID_START_ADDR;
    } else {
      *cmd->ret = eraseSector(cmd->flashAddress, DRIV_QSPI_ERASE_32K);
    }
    break;
  case DRIV_QSPI_ERASE_64K:
    if ((uint32_t)cmd->flashAddress % 0x10000) {
      *cmd->ret = DRIV_QSPI_ERR_INVALID_START_ADDR;
    } else {
      *cmd->ret = eraseSector(cmd->flashAddress, DRIV_QSPI_ERASE_64K);
    }
    break;
  default:
    *cmd->ret = DRIV_QSPI_ERR_INVALID_SIZE;
    break;
  }
  //debug_printf("processErase 0x%X %i\n",(uint32_t)cmd->flashAddress,cmd->length);
  return 0;
}

static void qspiTasks(void *pvParameters) {

  static uint32_t i;
  static QSPI_CMD xQueueReadCmd;
  static QSPI_CMD xQueueWriteCmd;
  //static uint32_t ulNotifiedValue;
  //static bool writeActive = false;
  //uint32_t ret;
  //memset(qspiTestOut, 0, 512);
  if (!drvInit()) {
    debug_printf("No Flash device found, trying again\n");
    if (!drvInit()) {
      debug_printf("No Flash device found\n");
      drivStatus = DRIV_QSPI_STATUS_INIT_ERR;
      vTaskDelay(portMAX_DELAY);
    }
  }
  drivStatus = DRIV_QSPI_STATUS_READY;
  debug_printf("Flash ready");
  //for (i = 0; i < 128; i++) {
  //  qspiTestIn[i] = i + 1;
  //}
  //ret = writeFlashPage((uint8_t *)0x2000000, qspiTestIn, 128);
  //ret = eraseFlash((uint8_t *)0x2000000, DRIV_QSPI_ERASE_4K);
  //ret = readFlash((uint8_t *)0x2000000, qspiTestOut, 384);
  while (1) {

    xQueueReceive(xQueueCmd, &xQueueReadCmd, portMAX_DELAY);
    if (xQueueReadCmd.command == QSPI_ACTION_READ) {
      processRead(&xQueueReadCmd);
      xTaskNotifyGive(xQueueReadCmd.xCallingTask);
      continue;
    }
    if (xQueueReadCmd.command == QSPI_ACTION_WRITE) {
      memcpy(&xQueueWriteCmd, &xQueueReadCmd, sizeof(xQueueWriteCmd));
      while (processWrite(&xQueueWriteCmd)) {
        if (pdTRUE == xQueuePeek(xQueueCmd, &xQueueReadCmd, 0)) {
          if (xQueueReadCmd.command == QSPI_ACTION_READ) {
            xQueueReceive(xQueueCmd, &xQueueReadCmd, 0);
            processRead(&xQueueReadCmd);
            xTaskNotifyGive(xQueueReadCmd.xCallingTask);
          }
        }
      }
      xTaskNotifyGive(xQueueWriteCmd.xCallingTask);

    } else if (xQueueReadCmd.command == QSPI_ACTION_ERASE) {
      memcpy(&xQueueWriteCmd, &xQueueReadCmd, sizeof(xQueueWriteCmd));
      while (processErase(&xQueueWriteCmd)) {
        if (pdTRUE == xQueuePeek(xQueueCmd, &xQueueReadCmd, 0)) {
          if (xQueueReadCmd.command == QSPI_ACTION_READ) {
            xQueueReceive(xQueueCmd, &xQueueReadCmd, 0);
            processRead(&xQueueReadCmd);
            xTaskNotifyGive(xQueueReadCmd.xCallingTask);
          }
        }
      }
      xTaskNotifyGive(xQueueWriteCmd.xCallingTask);
    }
  }
}

void QUADSPI_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (QUADSPI->SR & QUADSPI_SR_TCF) {
    QUADSPI->FCR = QUADSPI_FCR_CTCF; //Clear flag
    if (qspiAction == QSPI_ACTION_WRITE || qspiAction == QSPI_ACTION_ERASE) {
      //Set up auto polling to flag us when device is not busy.
      QUADSPI->PSMKR = SPI_FLASH_WIP;
      QUADSPI->PSMAR = 0;
      if (qspiAction == QSPI_ACTION_WRITE) {
        QUADSPI->PIR = QSPI_PAGEP_POLL_INTERVAL;
      } else {
        QUADSPI->PIR = QSPI_ERASE_POLL_INTERVAL;
      }
      QUADSPI->CR |= (QUADSPI_CR_SMIE | QUADSPI_CR_PMM | QUADSPI_CR_APMS);
      QUADSPI->DLR = 0;
      QUADSPI->CCR =
          QSPI_CCR_SIOO |
          QSPI_CCR_DMODE(QSPI_CCR_4LINES) |
          QSPI_CCR_INSTR(SPI_FLASH_INS_RDSR) |
          QSPI_CCR_IMODE(QSPI_CCR_4LINES) |
          QSPI_CCR_FMODE_AP;
    }
  }
  if (QUADSPI->SR & QUADSPI_SR_SMF) {
    QUADSPI->FCR = QUADSPI_FCR_CSMF;
    QUADSPI->CR &= ~(QUADSPI_CR_SMIE);
    vTaskNotifyGiveFromISR(xqspiTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void DMA2_Stream2_IRQHandler(void) {

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (LL_DMA_IsActiveFlag_TC2(DMA2)) {
    LL_DMA_ClearFlag_TC2(DMA2);
    /* Call function Tranmission complete Callback */
    QUADSPI->CR &= ~(QUADSPI_CR_DMAEN);
    if (qspiAction == QSPI_ACTION_READ) {
      vTaskNotifyGiveFromISR(xqspiTaskHandle, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else if (qspiAction == QSPI_ACTION_WRITE || qspiAction == QSPI_ACTION_ERASE) {

    } else {
      configASSERT(qspiAction != QSPI_ACTION_NONE);
    }
  } else if (LL_DMA_IsActiveFlag_TE2(DMA2)) {
    /* Call Error function */
    //__asm__("BKPT");
    //SPI_TransferError_Callback();
  }
}