/*
 * sd.h
 */
#include "knob.h"

#ifndef INC_SD_H_
#define INC_SD_H_

SD_HandleTypeDef hsd;

void SD_LoadPreset(Knob *knobs, char* filename);
void SD_Toggle();
void SD_Enable();
void SD_Disable();

#endif /* INC_SD_H_ */
