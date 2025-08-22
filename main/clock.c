#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"

#include "8segDisplayLib.h"

uint32_t counter;

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

void countingTask(void *pvParameters) {
  while(1){
    counter++;
    if (counter > 9){
      counter = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  
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
    displaySingleNum(segments1, counter);
    displaySingleNum(segments2, 0b11111111);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    //displayNums(segments1, digit1, nums1 , 2);
    //displayNums(segments2, digit2, nums2, 2);
  
  }

}




  

