/*
 * led.c
 */

#include "led.h"

const uint16_t LEDPins[] = { LED_1_Pin, LED_2_Pin, LED_3_Pin, LED_4_Pin, LED_5_Pin, LED_6_Pin };

void LED_On(uint8_t i) {
    HAL_GPIO_WritePin(GPIO_PORT_LEDS, LEDPins[i], GPIO_PIN_SET);
}

void LED_Off(uint8_t i) {
    HAL_GPIO_WritePin(GPIO_PORT_LEDS, LEDPins[i], GPIO_PIN_RESET);
}

void LED_AllOff() {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        LED_Off(i);
    }
}

void LED_Toggle(uint8_t i) {
    HAL_GPIO_TogglePin(GPIO_PORT_LEDS, LEDPins[i]);
}


