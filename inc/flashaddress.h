/* 
 * File:   flashaddress.h
 * Author: Erik
 *
 * Created on December 11, 2017, 7:04 PM
 */

#ifndef FLASHADDRESS_H
#define	FLASHADDRESS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define FLASH_APPLICATION_START   0x08018000
#define FLASH_APPLICATION_END     0x081FFF00
#define FLASH_APPLICATION_LEN     (FLASH_APPLICATION_END - FLASH_APPLICATION_START)

#define EXTERNAL_DATA_START   0x90000000
#define EXTERNAL_DATA_LEN     (14 * 1024 * 1024)
#define EXTERNAL_DATA_END     (0x90000000 + EXTERNAL_DATA_LEN)

typedef struct {
  uint32_t magicNumber;
  uint32_t length;
} _externalImage;

#define EXTERNAL_IMAGE_MAGIC  0x474D494E
#define EXTERNAL_IMAGE_START  0x93000000
#define EXTERNAL_IMAGE_LEN    (0x01000000 - sizeof(_externalImage))

#define FLASH_LOGS_ADDRESS    0x1000000UL//16MB
#define FLASH_LOGS_LENGTH     0x0200000UL//2MB
#define FLASH_CONFIG          0x1200000UL//18MB
#define FLASH_CONFIG_LENGTH   0x0200000UL//2MB
#define FLASH_FIRMWARE        0x2000000UL//32MB
#define FLASH_FIRMWARE_LEN    0x2000000UL//32MB
    
#ifdef	__cplusplus
}
#endif

#endif	/* FLASHADDRESS_H */
