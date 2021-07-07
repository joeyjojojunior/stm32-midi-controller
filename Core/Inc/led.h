/*
 * led.h
 */

#include <stdint.h>
#include "main.h"

#ifndef INC_LED_H_
#define INC_LED_H_

#define NUM_LEDS 6

extern uint16_t LEDPins[];

void LED_On(uint8_t i);
void LED_Off(uint8_t i);
void LED_AllOff();
void LED_Toggle(uint8_t i);

#endif /* INC_LED_H_ */
