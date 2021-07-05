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

void Preset_Load(Knob *knobs, char* buffer);

#endif /* INC_PRESET_H_ */
