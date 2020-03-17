#ifndef _DRIVQSPI_H_
#define _DRIVQSPI_H_

#include <stdint.h>
void drivQSPIinit(void);
uint32_t qspiDriverStatus(void);
uint32_t qspiReadFlash(uint8_t *flashAddress, uint8_t * data, uint32_t length);
uint32_t qspiEraseSector(uint8_t * flashAddress, uint32_t sectorLength);
uint32_t qspiWriteFlash(uint8_t * flashAddress, uint8_t * data, uint32_t length);

#define DRIV_QSPI_ERASE_4K  (1)
#define DRIV_QSPI_ERASE_32K (2)
#define DRIV_QSPI_ERASE_64K (3)

#define DRIV_QSPI_STATUS_READY     0
#define DRIV_QSPI_STATUS_INIT      1
#define DRIV_QSPI_STATUS_INIT_ERR  2

enum { DRIV_QSPI_ERR_NONE,
  DRIV_QSPI_ERR_INVALID_SIZE,
  DRIV_QSPI_ERR_INVALID_START_ADDR,
  DRIV_QSPI_ERR_BAD_PARAMS,
  DRIV_QSPI_ERR_READONLY,
  DRIV_QSPI_ERR_NULL_PTR,
  DRIV_QSPI_ERR_PAGE_ADDR };

#endif