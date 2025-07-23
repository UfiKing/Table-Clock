#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define D1 26
#define D2 25
#define D3 17
#define D4 16

#define A 18
#define B 23
#define C 14
#define D 13
#define E 5
#define F 19
#define G 27
#define DP 12


void app_main(void) { 
  const char *tag = "clock";

  ESP_LOGI(tag, "Initializing variables");


  const uint8_t hi = 1, lo = 0;
  const TickType_t duration = 500 / portTICK_PERIOD_MS;

  gpio_reset_pin(LED);
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);

  while (1){
    gpio_set_level(LED, hi);
    vTaskDelay(duration);
    gpio_set_level(LED, lo);
    vTaskDelay(duration);

  }

}
