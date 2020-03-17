
#include <stdlib.h>
#include <string.h>
#include "http/httpROMFS.h"

YAROMFSFILE_HANDLE *yaromfs_fopen(uint8_t *url, uint8_t *method, uint8_t *restData, uint32_t length) {
  const _yaROMFSFILE *handle = yaROMFSfind(url, method);
  if (handle == NULL) {
    return NULL;
  }
  YAROMFSFILE_HANDLE *ret = calloc(1, sizeof(YAROMFSFILE_HANDLE));
  if (ret) {
    ret->file = handle;
    ret->position = 0;
    if (handle->script_ptr != NULL) {
      ret->scriptResponse = handle->script_ptr(restData, length);
    }
  }
  return ret;
}

uint32_t yaromfs_fread(YAROMFSFILE_HANDLE *handle, uint8_t *buf, uint32_t len) {
  uint32_t ret;
  uint32_t remaining;
  if (handle->file->script_ptr != NULL) {
    /* Handle script file */
    remaining = handle->scriptResponse.len - handle->position;
    if (len > remaining) {
      len = remaining;
    } else if (len == 0) {
      return 0;
    }
    memcpy(buf, &handle->scriptResponse.data[handle->position], len);
    handle->position += len;
    return len;
  } else {
    remaining = handle->file->length - handle->position;
    uint8_t *flashAddress = (uint8_t *)handle->file->data;
    flashAddress += handle->position;
    if (len > remaining) {
      len = remaining;
    } else if (len == 0) {
      return 0;
    }
    if (ret = qspiReadFlash(flashAddress, buf, len)) {
      return 0;
    }
    handle->position += len;
    return len;
  }
}

uint32_t yaromfs_f_length(YAROMFSFILE_HANDLE *handle) {
  if (handle->file->script_ptr != NULL) {
    return handle->scriptResponse.len;
  } else {
    return handle->file->length;
  }
}

void yaromfs_fclose(YAROMFSFILE_HANDLE *handle) {
  if (handle != NULL) {
    if (handle->file->script_ptr != NULL) {
      if (handle->scriptResponse.data) {
        free(handle->scriptResponse.data);
      }
    }
    free(handle);
  }
}

uint32_t yaromfs_is_gz(YAROMFSFILE_HANDLE *handle) {
  if (handle->file->script_ptr != NULL) {
    return handle->scriptResponse.gz;
  } else {
    return handle->file->gz;
  }
}

const uint8_t * yaromfs_redirect(YAROMFSFILE_HANDLE *handle){
  return handle->file->contentType;
}

const uint8_t * yaromfs_contentType(YAROMFSFILE_HANDLE *handle){
  return handle->file->contentType;
}

uint32_t yaromfs_responseCode(YAROMFSFILE_HANDLE *handle) {
  if (handle->file->script_ptr != NULL) {
    return handle->scriptResponse.responseCode;
  } else {
    return 200;
  }
}

uint32_t yaromfs_postExists(uint8_t *url) {
  const _yaROMFSFILE *handle = yaROMFSfind(url, "POST");
  if (handle == NULL) {
    return 0;
  }
  return handle->length;
}

uint32_t yaromfs_preservePost(YAROMFSFILE_HANDLE *handle) {
  return handle->scriptResponse.preservePOST;
}