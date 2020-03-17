/* Processed by yaROMFS version 1.02 */
/* yaRomfsCore.c must be included  */
#include <stdint.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include "http/httpROMFS.h"

#include "yaROMFSconfig.h"

extern const uint8_t yaROMFSDAT[];
static const _yaROMFSFILE * lookup(uint8_t * url, uint8_t * method);

const _yaROMFSFILE * yaROMFSfind(uint8_t *url, uint8_t *method) {
    return lookup(url, method);
}

static uint32_t hash(unsigned char *str, unsigned char *method) {
    uint32_t hash = 5381;
    int c;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    while (c = *method++) {
        c = toupper(c);
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

static const _yaROMFSFILE fileList[] = {
    { .hash = 0xC2EFDA16, .contentType = "json", .length = 0x1000000, .script_ptr = NULL, .method = YAROMFS_POST }, /* POST /api/firmware.php */
    { .hash = 0x5CC2F917, .contentType = "json", .length = 0, .script_ptr = NULL, .method = YAROMFS_GET }, /* GET /api/status.php */
    { .hash = 0xC5322AE5, .contentType = "js", .length = 0, .script_ptr = NULL, .method = YAROMFS_GET }, /* GET /version.js */
    { .hash = 0x9E78D2AF, .contentType = "html", .data = &yaROMFSDAT[0x0], .length = 0x669, .gz = 1 },/* /index.html */
    { .hash = 0xB484DA7B, .contentType = "css", .data = &yaROMFSDAT[0x66A], .length = 0x46A, .gz = 1 },/* /css/bootmin.css */
    { .hash = 0x7D622C1B, .contentType = "txt", .data = &yaROMFSDAT[0xAD4], .length = 0x323, .gz = 1 },/* /bsd.txt */
};


static const _yaROMFSFILE * lookup(uint8_t * url, uint8_t * method){
	switch (hash(url, method)){
    case 0xC2EFDA16:
        return &fileList[0];
    case 0x5CC2F917:
        return &fileList[1];
    case 0xC5322AE5:
        return &fileList[2];
    case 0x9E78D2AF:
        return &fileList[3];
    case 0xB484DA7B:
        return &fileList[4];
    case 0x7D622C1B:
        return &fileList[5];
    default:
        return NULL;
	}
}

