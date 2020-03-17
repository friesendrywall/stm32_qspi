#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
//#include <__cross_studio_io.h>

#include "firmware.h"
#include "global.h"
#include "http/httpROMFS.h"

_httpResponse firmware_post_php(uint8_t *data, uint32_t len) {
  _httpResponse resp = { .responseCode = 200, .gz = 0, .preservePOST = 1 };
  uint8_t *response;
  if (startUpgrade(data, len, &response)) {
    resp.responseCode = 500;
  }
  resp.data = malloc(1024);
  resp.len = sprintf(resp.data, "{\"status\":\"%s\"}", response);
  return resp;
}
_httpResponse status_get_php(uint8_t *data, uint32_t len) {
  _httpResponse resp = { .responseCode = 200, .gz = 0 };
  resp.data = malloc(1024);
  xSemaphoreTake(statusMutex, portMAX_DELAY);
  resp.len = sprintf(resp.data,
      "{\"status\":\"%s\",\"complete\":\"%s\"}",
      firmwareStatus, firmwareComplete);
  xSemaphoreGive(statusMutex);
  return resp;
}

_httpResponse version_get_js(uint8_t *data, uint32_t len) {
  _httpResponse resp = { .responseCode = 200, .gz = 0 };
  resp.data = malloc(512);
  resp.len = sprintf(resp.data, "document.getElementById('BootloaderVersion').innerHTML = 'Bootloader Version %s';", BOOTLOADER_VERSION);
  return resp;
}