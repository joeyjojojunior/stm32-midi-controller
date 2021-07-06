/*
 * preset.h
 */

#ifndef INC_PRESET_H_
#define INC_PRESET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "knob.h"

char presets[NUM_KNOBS][MAX_LABEL_CHARS+1];

void Preset_GetName(char *file_buffer, char *name_buffer);
void Preset_Load(Knob *knobs, char* buffer);

#endif /* INC_PRESET_H_ */
