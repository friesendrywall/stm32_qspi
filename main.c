
#include <stm32f769xx.h>
#include <__cross_studio_io.h>

/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "list.h"
#include "init.h"
#include "semphr.h"
#include "task.h"
#include "http/httpROMFS.h"
#include "qspi/drivQSPI.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_crc.h"
#include "stm32f7xx_hal.h"
#include "http/httpROMFS.h"

#include "global.h"


const int __attribute__((used)) uxTopUsedPriority = configMAX_PRIORITIES - 1;

SemaphoreHandle_t debugMutex = NULL;
SemaphoreHandle_t printfMutex = NULL;
SemaphoreHandle_t ConfigMutex = NULL;
SemaphoreHandle_t PingMutex = NULL;
SemaphoreHandle_t ConsoleSingleMutex;
SemaphoreHandle_t statusMutex = NULL;

void __debug_io_lock(void) {

  if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
    xSemaphoreTake(debugMutex, portMAX_DELAY);
  }
}
void __debug_io_unlock(void) {
  if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
    xSemaphoreGive(debugMutex);
  }
}

void __printf_lock(void) {

  if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
    xSemaphoreTake(printfMutex, portMAX_DELAY);
  }
}
void __printf_unlock(void) {
  if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
    xSemaphoreGive(printfMutex);
  }
}

void testQSPI(void *params) {
  static uint8_t buf[128];
  while (qspiDriverStatus() != DRIV_QSPI_STATUS_READY) {
    vTaskDelay(10);
  }
  debug_printf("Opening file\n");
  YAROMFSFILE_HANDLE *f = yaromfs_fopen("/index.html", "GET", NULL, 0);
  yaromfs_fread(f, buf, 64);
  yaromfs_fclose(f);
  debug_printf("File read, etc 0x%X 0x%X\n", buf[0], buf[1]);

  while (1) {
    vTaskDelay(10);
  }
}

void main(void) {

  int a;
  /* We need these lines to prevent fader movement on debug stop */
  DBGMCU->APB2FZ |= DBGMCU_APB2_FZ_DBG_TIM1_STOP;
  initializeMinSystem();
  initializeSystem();

  drivQSPIinit();
  (void)xTaskCreate(testQSPI, "testqspi", 4000, NULL, 1, NULL);
  debugMutex = xSemaphoreCreateMutex();
  configASSERT(debugMutex != NULL);
  printfMutex = xSemaphoreCreateMutex();
  configASSERT(printfMutex != NULL);
  vTaskStartScheduler();

  for (;;)
    ;
}

void vApplicationMallocFailedHook(void) {
  /* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

  /* Force an assert. */
  configASSERT((volatile void *)NULL);
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
  (void)pcTaskName;
  (void)pxTask;

  /* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */

  /* Force an assert. */
  configASSERT((volatile void *)NULL);
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void) {
  volatile size_t xFreeHeapSpace;

  /* This is just a trivial example of an idle hook.  It is called on each
	cycle of the idle task.  It must *NOT* attempt to block.  In this case the
	idle task just queries the amount of FreeRTOS heap that remains.  See the
	memory management section on the http://www.FreeRTOS.org web site for memory
	management options.  If there is a lot of heap memory free then the
	configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
	RAM. */
  //xFreeHeapSpace = xPortGetFreeHeapSize();
  __NOP();
  /* Remove compiler warning about xFreeHeapSpace being set but never used. */
  (void)xFreeHeapSpace;
}
/*-----------------------------------------------------------*/

void vAssertCalled(uint32_t ulLine, const char *pcFile) {
  volatile unsigned long ul = 0;

  (void)pcFile;
  (void)ulLine;

  taskENTER_CRITICAL();
  {
    /* Set ul to a non-zero value using the debugger to step out of this
		function. */
    while (ul == 0) {
      __NOP();
    }
  }
  taskEXIT_CRITICAL();
}

/*-----------------------------------------------------------*/