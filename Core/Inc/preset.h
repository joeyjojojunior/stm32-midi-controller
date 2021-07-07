/*
 * preset.h
 */

#ifndef INC_PRESET_H_
#define INC_PRESET_H_

#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "knob.h"

extern char presetNames[NUM_KNOBS][MAX_LABEL_CHARS+1];

void Preset_GetName(char *file_buffer, char *name_buffer);
void Preset_Load(char* buffer);
char *Preset_Save(char* preset_name);

#endif /* INC_PRESET_H_ */
