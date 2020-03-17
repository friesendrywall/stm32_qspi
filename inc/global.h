#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

#define BOOTLOADER_VERSION "1.01"

#define HAL_TICK_FREQ (1000) //Hack, but stock API is weird

typedef struct {
  int LogLevel;
} _Info;

typedef struct {
  TaskHandle_t *handle;
  const char *name;
} _StackMeasure;

#define BOOT_LEVEL_MINIMAL 1
#define BOOT_LEVEL_NETWORK 2
#define BOOT_LEVEL_SEARCH  3

typedef uint32_t _Timer;
extern volatile _Info Info;
extern volatile uint32_t bootLevel;

extern SemaphoreHandle_t ConfigMutex;
extern SemaphoreHandle_t PingMutex;
extern SemaphoreHandle_t ConsoleSingleMutex;
extern SemaphoreHandle_t statusMutex;
extern TaskHandle_t xLocalIdleHandle;

#endif