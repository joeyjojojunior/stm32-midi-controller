/*
 * button.h
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#define NUM_BUTTONS 6
#define GPIO_PORT_BUTTONS_1TO5 GPIOA
#define GPIO_PORT_BUTTON_6 Button_6_GPIO_Port

extern volatile bool btnDown[];

void Button_Debounce();
bool Button_IsDown(uint8_t i);
void Button_Ignore(uint8_t i);

#endif /* INC_BUTTON_H_ */
