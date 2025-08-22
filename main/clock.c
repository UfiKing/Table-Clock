#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "8segDisplayLib.h"
#include "esp_sntp.h"
#include "esp_netif"
#include "esp_event.h"
#include "esp_wifi.h"

#include "credentials.h"

uint8_t counter = 00;
uint8_t minutes = 0;
uint8_t hours = 0;


void singleDigitExample1(uint8_t segments[8], uint32_t delay){
  
  TickType_t delayProccesed = delay / portTICK_PERIOD_MS;

  initSingleDigit(segments);
  while(1){
    for(uint8_t i = 0; i < 10; i++){
      displaySingleNum(segments, i);
      vTaskDelay(delayProccesed);
    }
      
  }
}


void initTimeServer() {
  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
  esp_netif_sntp_init(&config);
}

void getTime() {
  initTimeServer();

  uint8_t retry = 0;

  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && retry < 10){
    ESP_LOGI("time sync", "waiting for the time server to sync");
    retry++;
  }

  time_t now = 0;
  struct tm timeInfo = { 0 };
  time(&now);
  localtime_r(&now, &timeinfo);

}

void countingTask(void *pvParameters) {
  while(1){
    counter++;
    if (counter > 59){
      counter = 0;
      minutes++;
      if (minutes == 60){
        hours++;
        minutes = 0;
        if (hours == 24){
          hours = 0;
        }
      }
    }
    vTaskDelay(10   / portTICK_PERIOD_MS);
  
  }
}


void app_main(void){
  //                      A   B   C  D   E   F   G   DP  
  uint8_t segments1[8] = {27, 14, 1, 26, 33, 12, 25, 3};
  uint8_t segments2[8] = {4, 22, 19, 17, 5, 21, 16, 18};
  uint8_t digit1[2] = {13, 23};
  uint8_t digit2[2] = {2, 15};
  uint8_t nums1[2] = {1, 2};
  uint8_t nums2[2] = {3, 4};
  initMultipleSegments(segments1, digit1, 2);
  initMultipleSegments(segments2, digit2, 2);

  displaySingleNum(segments1, 0b1000000);
  displaySingleNum(segments2, 0b1000000);
  //vTaskDelay(1000 / portTICK_PERIOD_MS);  
  gpio_set_level(13, 0);
  gpio_set_level(23, 0);
  gpio_set_level(15, 0);
  gpio_set_level(2, 0);
 
  getTime();

  xTaskCreatePinnedToCore(
    countingTask, //function
    "Counter", //name
    2048, //stack size in bytes 
    NULL, //paramaters
    1, //priority
    NULL, // task handle
    0  //core id
  );

  while(1){


    turnOnDigit(digit1[0]);
    turnOnDigit(digit2[0]);
    displaySingleNum(segments1, hours / 10); 
    displaySingleNum(segments2, minutes / 10);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
    turnOffDigit(digit1[0]);
    turnOffDigit(digit2[0]);
    
    turnOnDigit(digit1[1]);
    turnOnDigit(digit2[1]); 
    
    displaySingleNum(segments1, hours % 10);
    displaySingleNum(segments2, minutes % 10);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
    turnOffDigit(digit1[1]);
    turnOffDigit(digit2[1]);


    //displayNums(segments1, digit1, nums1 , 2);
    //displayNums(segments2, digit2, nums2, 2);
  
  }

}




  







