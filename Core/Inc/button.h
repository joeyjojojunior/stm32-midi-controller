/*
 * button.h
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

extern volatile bool btnDown[];

void Button_Debounce();
bool Button_IsDown(uint8_t i);
void Button_Ignore(uint8_t i);

#endif /* INC_BUTTON_H_ */
