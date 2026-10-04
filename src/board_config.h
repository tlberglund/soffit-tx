
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "pico/stdlib.h"

#define MAGIC_NUMBER 0x2b00b1e5

typedef struct {
   uint32_t magic;
   char service_uuid[40];
   char char_uuid[40];
   char device_name[12];
   uint32_t strip_length;
   bool invert;
   uint8_t unused_1[3];
   uint32_t crc;
} BoardConfig;


extern BoardConfig *get_board_config();

#endif
