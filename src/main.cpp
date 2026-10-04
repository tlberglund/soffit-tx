#include <cstdio>
#include "pico/stdlib.h"
#include "board.h"

extern "C" {
    #include "FreeRTOSConfig.h"
    #include "FreeRTOS.h"
}











#if 0

static void *current_connection;

err_t httpd_post_begin(void *connection, 
                       const char *uri, 
                       const char *http_request,
                       u16_t http_request_len, 
                       int content_len, 
                       char *response_uri,
                       u16_t response_uri_len, 
                       u8_t *post_auto_wnd) {

   if(memcmp(uri, "/strip", 6) == 0 && current_connection != connection) {
      current_connection = connection;
      led_state_len = 0;
      led_state_current = led_state_buffer;
      snprintf(response_uri, response_uri_len, "/");
      *post_auto_wnd = 1;
      return ERR_OK;
   }
   return ERR_VAL;
}

// Return a value for a parameter
char *httpd_param_value(struct pbuf *p, 
                        const char *param_name, 
                        char *value_buf, 
                        size_t value_buf_len) {
   size_t param_len = strlen(param_name);
   u16_t param_pos = pbuf_memfind(p, param_name, param_len, 0);
   if(param_pos != 0xffff) {
      u16_t param_value_pos = param_pos + param_len;
      u16_t param_value_len = 0;
      u16_t tmp = pbuf_memfind(p, "&", 1, param_value_pos);
      if(tmp != 0xffff) {
         param_value_len = tmp - param_value_pos;
      } else {
         param_value_len = p->tot_len - param_value_pos;
      }
      if(param_value_len > 0 && param_value_len < value_buf_len) {
         char *result = (char *)pbuf_get_contiguous(p, value_buf, value_buf_len, param_value_len, param_value_pos);
         if(result) {
               result[param_value_len] = 0;
               return result;
         }
      }
   }
   return NULL;
}


err_t httpd_post_receive_data(void *connection, struct pbuf *p) {
   err_t ret = ERR_VAL;
   LWIP_ASSERT("NULL pbuf", p != NULL);
   printf("POST RX len=%d, tot_len=%d\n", p->len, p->tot_len);
   printf("%s\n", p->payload);

   if(current_connection == connection && 
      (p->len + led_state_len) < LED_STATE_BUFSIZE) {
      memcpy(led_state_current, p->payload, p->len);
      led_state_current += p->len;
      printf("%s\n", led_state_buffer);
      ret = ERR_OK;
   }

   pbuf_free(p);
   return ret;
}


void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len) {
   snprintf(response_uri, response_uri_len, "/error.html");
   if(current_connection == connection) {
      snprintf(response_uri, response_uri_len, "/");
      printf("POST FINISHED\n");

      size_t led_decoded_len = LED_DECODED_BUFSIZE;
      base64_decode(led_state_buffer, led_decoded_buffer, &led_decoded_len);

      printf("DECODED %d bytes for %d LEDs\n", led_decoded_len, led_strip.get_strip_len());

      for(int n = 0; n < led_strip.get_strip_len(); n++) {
         uint8_t brightness, red, green, blue;
         uint8_t *led_config;
         led_config = &led_decoded_buffer[n * 4];
         brightness = led_config[0];
         red = led_config[1];
         green = led_config[2];
         blue = led_config[3];
         led_strip.set_led(n, red, green, blue, brightness);
      }
      led_strip.update_strip();
   }
   current_connection = NULL;
}





void led_task(void *pvParameters) {
   WifiConnection *wifi = (WifiConnection *)pvParameters;

   printf("LED TASK STARTED, WAITING FOR WIFI\n");

   wifi->wait_for_wifi_init();

   printf("LED TASK WIFI IS UP\n");

   // http_set_cgi_handlers(cgi_handlers, LWIP_ARRAYSIZE(cgi_handlers));
   // httpd_init();

   for(;;) {
      vTaskDelay(1000);
   }
}
#endif






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
