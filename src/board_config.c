
#include <string.h>
#include "board_config.h"

#define SERVICE_UUID "a7f2d9e1-3c4b-4a8e-b5c6-1f2e3d4c5b6a"
#define CHAR_UUID "b8e3c9f2-4d5c-4b9f-c6d7-2e3f4d5c6b7a"
#define DEVICE_NAME "strip01"

char static_board_config[sizeof(BoardConfig)];
BoardConfig *config = NULL;

BoardConfig *get_board_config() {
   if(config == NULL) {
      config = (BoardConfig *)static_board_config;
      strcpy(config->service_uuid, SERVICE_UUID);
      strcpy(config->char_uuid, CHAR_UUID);
      strcpy(config->device_name, DEVICE_NAME);
      config->strip_length = 63;
      config->invert = false;
   }
   return config;
}