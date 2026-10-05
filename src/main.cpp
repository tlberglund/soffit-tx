#include <cstdio>
#include "pico/stdlib.h"
#include "board.h"

extern "C" {
    #include "FreeRTOSConfig.h"
    #include "FreeRTOS.h"
}






int main() {
   stdio_init_all();
   sleep_ms(2000);
   printf("UP\n");
   sleep_ms(500);
   printf("LAUNCHING\n");

   Board &board = Board::getInstance();
   board.init();

   vTaskStartScheduler();
}
