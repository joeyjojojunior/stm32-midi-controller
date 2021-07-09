/*
 * sd.h
 */
#include <stdlib.h>
#include <stdbool.h>
#include "fatfs.h"
#include "knob.h"
#include "preset.h"

#ifndef INC_SD_H_
#define INC_SD_H_

#define NUM_FN_RANDOM_CHARS 4
#define NUM_EXTENSION_CHARS 5

extern SD_HandleTypeDef hsd;
extern bool isPresetFilenamesLoaded;

bool SD_FetchPresetNames();
bool SD_LoadPreset(char *filename);
bool SD_SavePreset();
void getRandomFileNameNoExt(char *fileNameBuf);
void getRandomString(char *randBuf);

#endif /* INC_SD_H_ */
