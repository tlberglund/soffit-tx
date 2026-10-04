
#include <stdio.h>
#include "board.h"

extern "C" {
   #include "task.h"
   #include "utils.h"
   #include "hardware/gpio.h"
   #include "pico/time.h"
   #include "utils.h"
}





void Board::status_task_timer_callback(TimerHandle_t timer) {
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   Board &board = Board::getInstance();
   vTaskNotifyGiveFromISR(board.statusTaskHandle, &xHigherPriorityTaskWoken);
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


uint8_t Board::getPulseLevel(uint32_t step) {
   return pulseCurve[step % PULSE_SAMPLES];
}



extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
   printf("STACK OVERFLOW in task: %s\n", pcTaskName);
   for(;;);
}




/**
 * Monitor conditions and drive the status LEDs accoringly.
 */
void Board::statusTask(void *parms) {
   printf("STATUS TASK STARTED\n");

   Board &board = Board::getInstance();

   board.statusTaskHandle = xTaskGetCurrentTaskHandle();
   board.statusTimerHandle = xTimerCreate("StatusTimer", pdMS_TO_TICKS(40), pdTRUE, 0, Board::status_task_timer_callback);
   if(board.statusTimerHandle != NULL) {
      xTimerStart(board.statusTimerHandle, 0);
   }

   board.stripSelfTest();

   // uint32_t frame = 0;
   uint8_t network_blink_index = 0;
   for(;;) {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

      // frame++;
      network_blink_index++;
      if(network_blink_index >= PULSE_SAMPLES) {
         network_blink_index = 0;
      }
   }
}


void Board::initBoardConfig() {
   config.strip_length = 60;
   config.invert = false;
}


int Board::interpolateRamp(int width, int height, int offset) {
   if(offset < 0) {
      offset = -offset;
   }
   if(offset > width) {
      offset = offset % width;
   }
   if(offset < (width / 2)) {
      return ((float)offset / (width / 2)) * height;
   }
   else {
      return ((float)(width - offset) / (width / 2)) * height;
   }
}

void Board::stripSelfTest() {
   // Ramp brightness from zero to full
   for(int j = 0; j <= SK6812::MAX_BRIGHTNESS; j++) {
      led_strip->set_all(255, 255, 255, j);
      led_strip->update_strip();
      vTaskDelay(10);
   }

   // Ramp brightness from full to zero
   for(int j = SK6812::MAX_BRIGHTNESS - 1; j >= 0 ; j--) {
      led_strip->set_all(255, 255, 255, j);
      led_strip->update_strip();
      vTaskDelay(10);
   }

   // Perform a few pretty color ramps real quick
   for(int i = 0; i < config.strip_length; i++) {
      for(int j = 0; j < config.strip_length; j++) {
         int r = interpolateRamp(config.strip_length, 255, j);
         int g = interpolateRamp(config.strip_length, 255, j + i - (config.strip_length / 2));
         int b = interpolateRamp(config.strip_length, 255, j + (config.strip_length / 2) - i);
         //printf("%3d\t%3d\t%3d\t%3d\t%3d\n", i, i % config.strip_length, r, g, b);
         led_strip->set_led(j, r, g, b);
      }
      led_strip->update_strip();
      vTaskDelay(10);
   }

   for(int i = 0; i < config.strip_length; i++) {
      for(int j = 0; j < config.strip_length; j++) {
         int r = interpolateRamp(config.strip_length, 255, j);
         int g = interpolateRamp(config.strip_length, 255, j + (config.strip_length / 2) - i);
         int b = interpolateRamp(config.strip_length, 255, j + i - (config.strip_length / 2));
         led_strip->set_led(j, r, g, b);
      }
      led_strip->update_strip();
      vTaskDelay(10);
   }

      for(int i = 0; i < config.strip_length; i++) {
      for(int j = 0; j < config.strip_length; j++) {
         int r = interpolateRamp(config.strip_length, 255, j);
         int g = interpolateRamp(config.strip_length, 255, j + i - (config.strip_length / 2));
         int b = interpolateRamp(config.strip_length, 255, j + (config.strip_length / 2) - i);
         led_strip->set_led(j, r, g, b);
      }
      led_strip->update_strip();
      vTaskDelay(10);
   }

   for(int i = 0; i < config.strip_length; i++) {
      for(int j = 0; j < config.strip_length; j++) {
         int r = interpolateRamp(config.strip_length, 255, j);
         int g = interpolateRamp(config.strip_length, 255, j + (config.strip_length / 2) - i);
         int b = interpolateRamp(config.strip_length, 255, j + i - (config.strip_length / 2));
         led_strip->set_led(j, r, g, b);
      }
      led_strip->update_strip();
      vTaskDelay(10);
   }

   // Ramp brightness from zero to full
   for(int j = 0; j <= SK6812::MAX_BRIGHTNESS; j++) {
      led_strip->set_all(255, 255, 255, j);
      led_strip->update_strip();
      vTaskDelay(10);
   }

   // Ramp brightness from full to zero
   for(int j = SK6812::MAX_BRIGHTNESS - 1; j >= 0 ; j--) {
      led_strip->set_all(255, 255, 255, j);
      led_strip->update_strip();
      vTaskDelay(10);
   }

   led_strip->clear_strip();
   led_strip->update_strip();
}


void Board::init() {
   printf("Board::init() called\n");

   initBoardConfig();

   sleep_ms(100);

   xTaskCreate(statusTask, "Board Status Task", 512, NULL, 0, NULL);

   vTaskStartScheduler();
}