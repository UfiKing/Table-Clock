#ifndef _8SEG_DISPLAY_LIB
#define _8SEG_DISPLAY_LIB
#include <stdio.h>

void setDelay(uint32_t newDelay);

void initSingleDigit(uint8_t segments[8]);

void initPin(uint8_t pin);  

void displaySingleNum(uint8_t segments[8], uint8_t number);

void initMultipleSegments(uint8_t segmentPins[8], uint8_t digitPins[], uint8_t numOfDigitPins);

void turnOnDigit(uint8_t pin);

void turnOffDigit(uint8_t pin);

void displayNums(uint8_t segmentPins[8], uint8_t digitPins[], uint8_t nums[], uint8_t len);

#endif

