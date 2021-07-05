/*
 * sd.h
 */
#include <stdlib.h>
#include "fatfs.h"
#include "knob.h"
#include "preset.h"

#define MAX_LFN_CHARS 255

#ifndef INC_SD_H_
#define INC_SD_H_

SD_HandleTypeDef hsd;

void SD_FetchPresets();
void SD_LoadPreset(Knob *knobs, char* filename);
void SD_Toggle();
void SD_Enable();
void SD_Disable();
int qsort_cmp(const void* lhs, const void* rhs);

#endif /* INC_SD_H_ */
