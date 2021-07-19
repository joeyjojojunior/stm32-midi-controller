/*
 * preset.h
 */

#ifndef INC_PRESET_H_
#define INC_PRESET_H_

#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "knob.h"
#include "ff.h"
#include "tiny-json.h"

#define MAX_PRESETS 1
#define MAX_FILENAME_LENGTH _MAX_LFN/8
#define MAX_KNOB_TOKENS 135

typedef struct Preset {
    char filename[MAX_FILENAME_LENGTH + 1];
    char name[MAX_KNOB_LABEL_CHARS + 1];
    char sub_label[MAX_KNOB_LABEL_CHARS + 1];
    uint16_t index;
} Preset;

extern uint16_t numPresets;
extern uint16_t currentPreset;
extern Preset presets[MAX_PRESETS];

void Preset_GetName(char *filename, char *file_buffer, char *name_buffer);
void Preset_Load(char* buffer);
char *Preset_Save(char* preset_name);
void tokenizer(char inputStr[], char delim, char buf[][MAX_KNOB_LABEL_CHARS + 1]);

#endif /* INC_PRESET_H_ */
