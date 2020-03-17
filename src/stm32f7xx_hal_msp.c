/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : stm32f7xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_conf.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_cortex.h"
//#include "stm32f7xx_hal_eth.h"
#include "stm32f7xx_hal_sdram.h"
//#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_hal_qspi.h"
#include "priorities.h"

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

 // __HAL_RCC_PWR_CLK_ENABLE();
 // __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* System interrupt init*/
  /* PendSV_IRQn interrupt configuration */
 // HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief QSPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hqspi: QSPI handle pointer
* @retval None
*/
void HAL_QSPI_MspInit(QSPI_HandleTypeDef* hqspi)
{
  LL_GPIO_InitTypeDef LL_GPIO_InitStruct = {0};
  if(hqspi->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();
  
   /* __HAL_RCC_GPIOE_CLK_ENABLE(); Redundant code
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();*/
    /**QUADSPI GPIO Configuration    
    PE2     ------> QUADSPI_BK1_IO2
    PB6     ------> QUADSPI_BK1_NCS
    PB2     ------> QUADSPI_CLK
    PD12     ------> QUADSPI_BK1_IO1
    PD13     ------> QUADSPI_BK1_IO3
    PD11     ------> QUADSPI_BK1_IO0 
    */
    LL_GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    LL_GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;


    LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    LL_GPIO_InitStruct.Alternate = LL_GPIO_AF_9;
    LL_GPIO_Init(GPIOE, &LL_GPIO_InitStruct);

    LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    LL_GPIO_InitStruct.Alternate = LL_GPIO_AF_10;
    LL_GPIO_Init(GPIOB, &LL_GPIO_InitStruct);

    LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    LL_GPIO_InitStruct.Alternate = LL_GPIO_AF_9;
    LL_GPIO_Init(GPIOB, &LL_GPIO_InitStruct);

    LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13;
    LL_GPIO_InitStruct.Alternate = LL_GPIO_AF_9;
    LL_GPIO_Init(GPIOD, &LL_GPIO_InitStruct);
  }
}

/**
* @brief QSPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hqspi: QSPI handle pointer
* @retval None
*/
void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef *hqspi) {
  LL_GPIO_InitTypeDef LL_GPIO_InitStruct = {0};
  if (hqspi->Instance == QUADSPI) {
    /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

    /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration    
    PE2     ------> QUADSPI_BK1_IO2
    PB6     ------> QUADSPI_BK1_NCS
    PB2     ------> QUADSPI_CLK
    PD12     ------> QUADSPI_BK1_IO1
    PD13     ------> QUADSPI_BK1_IO3
    PD11     ------> QUADSPI_BK1_IO0 
    */
    LL_GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    LL_GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_InitStruct.Alternate = LL_GPIO_AF_0;

    LL_GPIO_InitStruct.Pin = (LL_GPIO_PIN_2);
    LL_GPIO_Init(GPIOE, &LL_GPIO_InitStruct);

    LL_GPIO_InitStruct.Pin = (LL_GPIO_PIN_2 | LL_GPIO_PIN_6);
    LL_GPIO_Init(GPIOB, &LL_GPIO_InitStruct);

    LL_GPIO_InitStruct.Pin = (LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13);
    LL_GPIO_Init(GPIOD, &LL_GPIO_InitStruct);
  }
}

static uint32_t FMC_Initialized = 0;

static void HAL_FMC_MspInit(void){
  /* USER CODE BEGIN FMC_MspInit 0 */

  /* USER CODE END FMC_MspInit 0 */
  //GPIO_InitTypeDef GPIO_InitStruct ={0};
  LL_GPIO_InitTypeDef LL_GPIO_InitStruct = {0};
  if (FMC_Initialized) {
    return;
  }
  FMC_Initialized = 1;

  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_ENABLE();

  /** FMC GPIO Configuration  
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PG15   ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PF0   ------> FMC_A0
  PF1   ------> FMC_A1
  PF2   ------> FMC_A2
  PF3   ------> FMC_A3
  PG8   ------> FMC_SDCLK
  PF4   ------> FMC_A4
  PH5   ------> FMC_SDNWE
  PH3   ------> FMC_SDNE0
  PF5   ------> FMC_A5
  PH2   ------> FMC_SDCKE0
  PD15   ------> FMC_D1
  PD10   ------> FMC_D15
  PD14   ------> FMC_D0
  PD9   ------> FMC_D14
  PD8   ------> FMC_D13
  PF12   ------> FMC_A6
  PG1   ------> FMC_A11
  PF15   ------> FMC_A9
  PG2   ------> FMC_A12
  PF13   ------> FMC_A7
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PG5   ------> FMC_BA1
  PG4   ------> FMC_BA0
  PF14   ------> FMC_A8
  PF11   ------> FMC_SDNRAS
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PE14   ------> FMC_D11
  PE7   ------> FMC_D4
  PE10   ------> FMC_D7
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  PE13   ------> FMC_D10
  */

  LL_GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  LL_GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  LL_GPIO_InitStruct.Alternate = LL_GPIO_AF_12;

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_7 | LL_GPIO_PIN_8 |
                           LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 |
                           LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
  LL_GPIO_Init(GPIOE, &LL_GPIO_InitStruct);

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_4 |
                           LL_GPIO_PIN_5 | LL_GPIO_PIN_8 | LL_GPIO_PIN_15;
  LL_GPIO_Init(GPIOG, &LL_GPIO_InitStruct);

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_8 | LL_GPIO_PIN_9 |
                           LL_GPIO_PIN_10 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
  LL_GPIO_Init(GPIOD, &LL_GPIO_InitStruct);

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 |
                           LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 |
                           LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
  LL_GPIO_Init(GPIOF, &LL_GPIO_InitStruct);

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_3 | LL_GPIO_PIN_2;
  LL_GPIO_Init(GPIOH, &LL_GPIO_InitStruct);

}

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* hsdram){
  /* USER CODE BEGIN SDRAM_MspInit 0 */

  /* USER CODE END SDRAM_MspInit 0 */
  HAL_FMC_MspInit();
  /* USER CODE BEGIN SDRAM_MspInit 1 */

  /* USER CODE END SDRAM_MspInit 1 */
}

static uint32_t FMC_DeInitialized = 0;

static void HAL_FMC_MspDeInit(void) {

  LL_GPIO_InitTypeDef LL_GPIO_InitStruct = {0};

  if (FMC_DeInitialized) {
    return;
  }
  FMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_DISABLE();

  /** FMC GPIO Configuration  
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PG15   ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PF0   ------> FMC_A0
  PF1   ------> FMC_A1
  PF2   ------> FMC_A2
  PF3   ------> FMC_A3
  PG8   ------> FMC_SDCLK
  PF4   ------> FMC_A4
  PH5   ------> FMC_SDNWE
  PH3   ------> FMC_SDNE0
  PF5   ------> FMC_A5
  PH2   ------> FMC_SDCKE0
  PD15   ------> FMC_D1
  PD10   ------> FMC_D15
  PD14   ------> FMC_D0
  PD9   ------> FMC_D14
  PD8   ------> FMC_D13
  PF12   ------> FMC_A6
  PG1   ------> FMC_A11
  PF15   ------> FMC_A9
  PG2   ------> FMC_A12
  PF13   ------> FMC_A7
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PG5   ------> FMC_BA1
  PG4   ------> FMC_BA0
  PF14   ------> FMC_A8
  PF11   ------> FMC_SDNRAS
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PE14   ------> FMC_D11
  PE7   ------> FMC_D4
  PE10   ------> FMC_D7
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  PE13   ------> FMC_D10
  */
  LL_GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  LL_GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  LL_GPIO_InitStruct.Alternate = LL_GPIO_AF_0;

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_7 | LL_GPIO_PIN_8 |
                           LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 |
                           LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
  LL_GPIO_Init(GPIOE, &LL_GPIO_InitStruct);

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_4 |
                           LL_GPIO_PIN_5 | LL_GPIO_PIN_8 | LL_GPIO_PIN_15;
  LL_GPIO_Init(GPIOG, &LL_GPIO_InitStruct);

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_8 | LL_GPIO_PIN_9 |
                           LL_GPIO_PIN_10 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
  LL_GPIO_Init(GPIOD, &LL_GPIO_InitStruct);

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 |
                           LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 |
                           LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
  LL_GPIO_Init(GPIOF, &LL_GPIO_InitStruct);

  LL_GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_3 | LL_GPIO_PIN_2;
  LL_GPIO_Init(GPIOH, &LL_GPIO_InitStruct);

}

void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef* hsdram){
  /* USER CODE BEGIN SDRAM_MspDeInit 0 */

  /* USER CODE END SDRAM_MspDeInit 0 */
  HAL_FMC_MspDeInit();
  /* USER CODE BEGIN SDRAM_MspDeInit 1 */

  /* USER CODE END SDRAM_MspDeInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/