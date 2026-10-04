
#ifndef __SK6812_H__
#define __SK6812_H__

#include "pico.h"
#include "hardware/pio.h"
#include "hardware/dma.h"


typedef struct {
   unsigned int red : 8;
   unsigned int green : 8;
   unsigned int blue : 8;
   unsigned int white : 8;
} SK6812_LED;


class SK6812 {
   public:
      SK6812(uint strip_len) : SK6812(strip_len, DEFAULT_SK6812_DIN_PIN) {};
      SK6812(uint strip_len, uint8_t din_pin) : SK6812(strip_len, din_pin, pio0, 0) {};
      SK6812(uint strip_len, uint8_t din_pin, pio_hw_t *pio, uint sm) : SK6812(strip_len, din_pin, pio, sm, DEFAULT_FREQUENCY) {};
      SK6812(uint strip_len, uint8_t din_pin, pio_hw_t *pio, uint sm, uint32_t frequency);
      ~SK6812();

      SK6812_LED *get_strip() { return (strip); };
      uint32_t get_strip_len() { return (strip_len); };
      void set_led(uint16_t index, SK6812_LED led);
      void set_led(uint16_t index, uint8_t red, uint8_t green, uint8_t blue) { set_led(index, red, green, blue, 0); };
      void set_led(uint16_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t white);
      void set_all(uint8_t red, uint8_t green, uint8_t blue) { set_all(red, green, blue, 0); };
      void set_all(uint8_t red, uint8_t green, uint8_t blue, uint8_t white);
      void update_strip();
      void clear_strip();

      static const uint32_t DEFAULT_FREQUENCY = 1000000;
      static const uint8_t DEFAULT_SK6812_CLK_PIN = 2;
      static const uint8_t DEFAULT_SK6812_DIN_PIN = 3;
      static const uint8_t MAX_BRIGHTNESS = 31;


   private:
      pio_hw_t *pio;
      uint pio_state_machine;
      uint32_t clk_freq;
      uint8_t clk_pin;
      uint8_t din_pin;
      uint strip_len;
      uint dma_pio_tx;
      dma_channel_config sk6812_dma_channel_config;
      SK6812_LED *strip;


      uint32_t get_buffer_size();
};

#endif

