
#include <cstring>
#include <stdio.h>
#include "sk6812.h"
#include "ws2812.pio.h"


SK6812::SK6812(uint strip_len, uint8_t din_pin, pio_hw_t *pio, uint sm, uint32_t frequency) :
   strip_len(strip_len),
   din_pin(din_pin),
   clk_freq(frequency),
   pio_state_machine(sm),
   pio(pio)
{
   printf("CONSTRUCTING SK6812\n");
   printf("buffer size=%d\n", get_buffer_size());
   strip = new SK6812_LED[get_buffer_size()];
   printf("strip=%08x\n", strip);

   // Set start and end frames per APA102 protocol
   memset(strip, 0, sizeof(SK6812_LED));
   memset(&strip[strip_len + 1], 0xff, sizeof(SK6812_LED));

   // All LEDs off
   clear_strip();

   // Set up PIO state machine
   uint offset = pio_add_program(pio, &ws2812_program);
   ws2812_program_init(pio, pio_state_machine, offset, din_pin, clk_freq, true);

   printf("pio offset=%d\n", offset);

   // Point DMA to the PIO state machine
   dma_pio_tx = dma_claim_unused_channel(true);
   sk6812_dma_channel_config = dma_channel_get_default_config(dma_pio_tx);
   channel_config_set_transfer_data_size(&sk6812_dma_channel_config, DMA_SIZE_32);
   channel_config_set_dreq(&sk6812_dma_channel_config, pio_get_dreq(pio, pio_state_machine, true));
   printf("dma channel=%d\n", dma_pio_tx);
};


void SK6812::clear_strip() {
   for(int i = 1; i < strip_len + 1; i++) {
      strip[i].white = 0;
      strip[i].red = 0;
      strip[i].green = 0;
      strip[i].blue = 0;
   }
}


uint32_t SK6812::get_buffer_size() {
   // First and last four bytes are the start and end frames per SK6812 protocol
   return ((strip_len + 2) * sizeof(SK6812_LED));
}


void SK6812::update_strip() {
   dma_channel_configure(dma_pio_tx, 
                         &sk6812_dma_channel_config,
                         &pio->txf[pio_state_machine],  // write to PIO TX FIFO
                         strip,                         // read from LED buffer
                         (get_buffer_size() / 4) - 1,   // 32-bit DMA transfers, not sure why why we have to subtract one
                         true);
}


void SK6812::set_led(uint16_t index, SK6812_LED led) {
   if(index < strip_len) {
      strip[index + 1] = led;
   }
}


void SK6812::set_led(uint16_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {
   if(index < strip_len) {
      strip[index + 1].white = white;
      strip[index + 1].red = red;
      strip[index + 1].green = green;
      strip[index + 1].blue = blue;
   }
}


void SK6812::set_all(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {
   for(int i = 0; i < strip_len; i++) {
      set_led(i, red, green, blue, white);
   }
}

SK6812::~SK6812() {
   delete strip;
};
