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

#define LED 2
void app_main(void) { 
  const char *tag = "clock";

  ESP_LOGI(tag, "Initializing variables");
  
  const uint8_t digitON = 1, digitOFF = 0, segmentON = 0, segmentOFF = 1;
  const TickType_t duration = 500 / portTICK_PERIOD_MS;
  
  const uint8_t segmentPins[] = {A, B, C, D, E, F, G, DP};
  const uint8_t digitPins[] = {D1, D2, D3, D4};

  ESP_LOGI(tag, "Configuring GPIO pins");
  for (int i = 0; i < 8; i++){
    gpio_reset_pin(segmentPins[i]);
    gpio_set_direction(segmentPins[i], GPIO_MODE_OUTPUT);
    gpio_set_level(segmentPins[i], segmentOFF);
  }

  for (int i = 0; i < 4; i++){
    gpio_reset_pin(digitPins[i]);
    gpio_set_direction(digitPins[i], GPIO_MODE_OUTPUT);
    gpio_set_level(digitPins[i], digitOFF);
  }


  gpio_reset_pin(LED);
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    while (1){
    gpio_set_level(D1, digitON);
    gpio_set_level(B, segmentON);
    gpio_set_level(C, segmentON);
    gpio_set_level(LED, 1);
    vTaskDelay(duration);
    gpio_set_level(D1, digitOFF);
    gpio_set_level(B, segmentOFF);
    gpio_set_level(C, segmentOFF);
    gpio_set_level(LED, 0);
    vTaskDelay(duration);

  }

}
