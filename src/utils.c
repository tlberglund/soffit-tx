
#include <stdio.h>
#include "utils.h"


void print_buffer(uint8_t *buffer, uint16_t len) {
   int byteCount = 0;
   char rendered[16 * 3 + 1 + 16 + 1];
   while(byteCount < len) {
      int lineLen = (len - byteCount) < 16 ? (len - byteCount) : 16;
      char *p = rendered;
      for(int n = 0; n < lineLen; n++) {
         p += sprintf(p, "%02x ", buffer[byteCount + n]);
      }
      // Pad if line is shorter than 16 bytes
      for(int n = lineLen; n < 16; n++) {
         p += sprintf(p, "   ");
      }
      *p++ = ' ';
      for(int n = 0; n < lineLen; n++) {
         uint8_t c = buffer[byteCount + n];
         if(c >= 32 && c <= 126) {
            *p++ = c;
         }
         else {
            *p++ = '.';
         }
      }
      *p++ = '\0';
      printf("%s\n", rendered);
      byteCount += lineLen;
   }
}
