#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "protocol_examples_common.h"
#include "esp_wifi.h"

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

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



void showDigit(uint8_t* num, uint8_t* pins){

  for (int i = 0; i < 8; i++) {   
    if( (*num >> i) & 0b00000001){
      gpio_set_level(pins[i], 0);
    }else{
      gpio_set_level(pins[i], 1);
    }
  }
}


void app_main(void) { 
  const char *tag = "clock";
  
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  ESP_ERROR_CHECK(example_connect());

  ESP_LOGI(tag, "Initializing variables");
  
  time_t now;
  char strftime_buf[64];
  struct tm timeinfo;
  
  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(pool.ntp.otg);
  esp_netif_sntp_init(&config);

  if (esp_netif_sntp_sync_wait(pdMS_TO_TICK(10000)) != ESP_OK) {q
    ESP_LOGI(tag, "Failed to update system time within 10s timeout");
  }

  time(&now);

  setenv("TZ", "CEST-2", 1);
  tzset();

  localtime_r(&now, &timeinfo);

  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

  ESP_LOGI(tag, "The current time is: %s", strftime_buf);




  const uint8_t digitON = 1, digitOFF = 0, segmentON = 0, segmentOFF = 1;
  const TickType_t delay500 = 500 / portTICK_PERIOD_MS;
  const TickType_t delay10 = 10 / portTICK_PERIOD_MS;
  
  enum number {
    ONE = 0b00000110,
    TWO = 0b01011011,
    THREE = 0b01001111,
    FOUR = 0b01100110,
    FIVE = 0b01101101,
    SIX = 0b01111100,
    SEVEN = 0b00000111,
    EIGHT = 0b01111111 ,
    NINE = 0b01100111 ,
    ZERO = 0b00111111,
    OFF = 0b00000000
  };

  uint8_t segmentPins[] = {A, B, C, D, E, F, G, DP};
  uint8_t digitPins[] = {D1, D2, D3, D4};

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
  
  while (1){
    gpio_set_level(D1, digitON);
    showDigit(&one, segmentPins);
    vTaskDelay(delay10);
    gpio_set_level(D1, digitOFF);
    gpio_set_level(D2, digitON);
    showDigit(&two, segmentPins);
    vTaskDelay(delay10);
    gpio_set_level(D2, digitOFF);

    
  }

}

// FOR LEGACY 
/* const uint8_t one = 0b00000110;
  const uint8_t two = 0b01011011;
  const uint8_t three = 0b01001111;
  const uint8_t four = 0b01100110;
  const uint8_t five = 0b01101101;
  const uint8_t six = 0b01111100;
  const uint8_t seven = 0b00000111;
  const uint8_t eight = 0b01111111;
  const uint8_t nine = 0b01100111;
  const uint8_t zero = 0b00111111;
  const uint8_t off = 0b00000000;
*/

  

