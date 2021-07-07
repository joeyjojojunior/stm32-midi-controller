/*
 * button.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "button.h"
#include "main.h"

uint8_t btnCount[NUM_BUTTONS] = { 0 };
uint8_t btnState[NUM_BUTTONS] = { GPIO_PIN_SET };
uint16_t btnPins[] = { Button_1_Pin, Button_2_Pin, Button_3_Pin, Button_4_Pin, Button_5_Pin, Button_6_Pin };

void Button_Debounce() {
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        GPIO_TypeDef *t = (i == BUTTON_MENU) ? GPIO_PORT_BUTTON_6 : GPIO_PORT_BUTTONS_1TO5;
        uint8_t currentState = HAL_GPIO_ReadPin(t, btnPins[i]);

        if (currentState != btnState[i]) {
            btnCount[i]++;
            if (btnCount[i] >= 4) {
                btnState[i] = currentState;

                if (currentState != GPIO_PIN_SET)
                    btnDown[i] = true;

                btnCount[i] = 0;
            }
        } else {
            btnCount[i] = 0;
        }
    }
}

bool Button_IsDown(uint8_t i) {
    bool ret = btnDown[i];
    if (ret) {
        btnDown[i] = false;
        isDisplayLocked = false;
    }
    return ret;
}

void Button_Ignore(uint8_t i) {
    btnDown[i] = false;
}

