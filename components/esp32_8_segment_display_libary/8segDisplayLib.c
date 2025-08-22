#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdint.h>
#include "8segDisplayLib.h"

uint32_t delay =  9;

void setDelay(uint32_t newDelay){
  delay = newDelay;
}



void initSingleDigit(uint8_t segments[8]){
  for(uint8_t i = 0; i < 8; i++){
    initPin(segments[i]);

  }
  
} 

void initPin(uint8_t pin){

  gpio_config_t config = {
    .pin_bit_mask = (1ULL << pin),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
  };
  
  gpio_reset_pin(pin);
  
  gpio_config(&config);
  
  gpio_set_level(pin, 0);
}

void displaySingleNum(uint8_t segments[8], uint8_t number){

  switch(number){
    case 0:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b00111111 >> i) & 1); 
      }
      break;
    case 1:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b00000110 >> i) & 1); 
      }
      break;
    case 2:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b01011011 >> i) & 1); 
      }
      break;
    case 3:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b01001111 >> i) & 1); 
      }
      break;
    case 4:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b01100110 >> i) & 1); 
      }
      break;
    case 5:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b01101101 >> i) & 1); 
      } 
      break;
    case 6:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b01111101 >> i) & 1); 
      }
      break;
    case 7:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b00000111 >> i) & 1); 
      }
      break;
    case 8:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b01111111 >> i) & 1); 
      }
      break;
    case 9:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (0b01101111 >> i) & 1); 
      }
      break;
    default:
      for(uint8_t i = 0; i < 8; i++){
        gpio_set_level(segments[i], (number >> i) & 1);
      }
      break;
  }
}


void initMultipleSegments(uint8_t segmentPins[8], uint8_t digitPins[], uint8_t numOfDigitPins){
  
  for(int i = 0; i < 8; i++){
    
    initPin(segmentPins[i]);


  }

  for(int i = 0; i < numOfDigitPins; i++){
    initPin(digitPins[i]); 
  
    gpio_set_level(digitPins[i], 1);

  }
}

void turnOnDigit(uint8_t pin){
  gpio_set_level(pin, 0);
}

void turnOffDigit(uint8_t pin){
  gpio_set_level(pin, 1);
}



void displayNumsAny(uint8_t segmentPins[8], uint8_t digitPins[], uint8_t nums[], uint8_t len){

  for(int i = 0; i < len; i++){ 
    turnOnDigit(digitPins[i]);
    displaySingleNum(segmentPins, nums[i]);
    vTaskDelay(delay / portTICK_PERIOD_MS);
    turnOffDigit(digitPins[i]);
  }

}


void displayNums2(uint8_t segmentPins[8], uint8_t digitPins[2], uint8_t num){


  for(uint8_t i = 0; i < 8; i++){

    turnOnDigit(digitPins[0]);
    displaySingleNum(segmentPins, num / 10); 
    vTaskDelay(10 / portTICK_PERIOD_MS);
    turnOffDigit(digitPins[0]);
    turnOnDigit(digitPins[1]); 
    displaySingleNum(segmentPins, (num % 10));
    vTaskDelay(10 / portTICK_PERIOD_MS);
    turnOffDigit(digitPins[1]);


  }
}









 
