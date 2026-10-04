
#include "sk6812.h"

extern "C" {
   #include "FreeRTOSConfig.h"
   #include "FreeRTOS.h"
   #include "task.h"
   #include "timers.h"
}


#define MAX_STRIP_LEN 60

#define GPIO_STATUS_LED 10

// SK6812 configuration parameters for the main LED strip
#define STRIP_DATA_PIN 3
#define STRIP_PIO pio0
#define STRIP_SM 0

// Number of frames in the blink animation 
#define PULSE_SAMPLES 50

// Config structures for the strip controller board
#define MAGIC_NUMBER 0x2b00b1e5

typedef struct {
   uint32_t magic;
   uint32_t strip_length;
   bool invert;
   uint8_t unused_1[3];
   uint32_t crc;
} BoardConfig;

#define SERVICE_UUID "a7f2d9e1-3c4b-4a8e-b5c6-1f2e3d4c5b6a"
#define CHAR_UUID "b8e3c9f2-4d5c-4b9f-c6d7-2e3f4d5c6b7a"






class Board {
   public:
      void init();
      TaskHandle_t statusTaskHandle;

      static Board &getInstance() {
         static Board instance;
         return instance;
      };

      Board(const Board&) = delete;
      Board& operator=(const Board&) = delete;

   private:
      Board() : frameCount(0),
                stripBrightness(15) {
         led_strip = new SK6812(60, STRIP_DATA_PIN, STRIP_PIO, STRIP_SM);
      };

      static void statusTask(void *parms);
      static void status_task_timer_callback(TimerHandle_t timer);

      void initBoardConfig();
      void stripSelfTest();
      int interpolateRamp(int width, int height, int offset);

      SK6812 *led_strip;
      volatile uint64_t frameCount;
      volatile uint8_t stripBrightness;
      BoardConfig config;
      TimerHandle_t statusTimerHandle;

      uint8_t pulseCurve[PULSE_SAMPLES] = { 
         0, 0, 0, 0, 0, 1, 4, 15, 46, 109, 192, 255, 255, 192, 109, 46, 15, 4, 1, 0, 0, 0, 0, 0, 0, 
         0, 0, 0, 0, 0, 0, 0,  0,  0,   0,   0,   0,   0,   0,   0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0 
      };

      uint8_t getPulseLevel(uint32_t step);
};