/*
 * sd.c
 */
#include <time.h>
#include "sd.h"
#include "ssd1306.h"

bool isPresetFilenamesLoaded = false;

bool SD_FetchPresetNames() {
    DIR root;
    FILINFO root_info;

    retSD = f_mount(&SDFatFS, "", 1);
    if (retSD != FR_OK) return false;

    numPresets = 0;

    retSD = f_findfirst(&root, &root_info, "", "*.txt");
    while (retSD == FR_OK && root_info.fname[0]) {
        retSD = f_open(&SDFile, root_info.fname, FA_READ);

        char presetBuffer[f_size(&SDFile) + 1];
        char nameBuffer[MAX_KNOB_LABEL_CHARS + 1];
        unsigned int bytesRead;

        retSD = f_read(&SDFile, presetBuffer, sizeof(presetBuffer) - 1, &bytesRead);
        presetBuffer[bytesRead] = '\0';
        retSD = f_close(&SDFile);

        Preset_GetName(root_info.fname, presetBuffer, nameBuffer);

        retSD = f_findnext(&root, &root_info);
    }

    f_closedir(&root);
    retSD = f_mount(NULL, "", 0);

    return true;
}

bool SD_LoadPreset(char *filename) {
    retSD = f_mount(&SDFatFS, "", 1);
    retSD = f_open(&SDFile, filename, FA_READ | FA_WRITE);

    char presetBuffer[f_size(&SDFile) + 1];
    unsigned int bytesRead;
    retSD = f_read(&SDFile, presetBuffer, sizeof(presetBuffer) - 1, &bytesRead);
    presetBuffer[bytesRead] = '\0';
    retSD = f_close(&SDFile);
    retSD = f_mount(NULL, "", 0);

    Preset_Load(presetBuffer);

    return true;
}

bool SD_SavePreset() {
    /*
    if (numPresets >= MAX_PRESETS) return false;

    UINT bw;
    FILINFO root_info;
    char fileNameNoExtRand[MAX_FILENAME_LENGTH - NUM_EXTENSION_CHARS + 1];
    char filenameRand[strlen(fileNameNoExtRand) + NUM_EXTENSION_CHARS + 2];

    retSD = f_mount(&SDFatFS, "", 1);

    // Keep randomly generating filenames until we get a unique one
    do {
        getRandomFileNameNoExt(fileNameNoExtRand);
        snprintf(filenameRand, strlen(fileNameNoExtRand) + 2, "%s", fileNameNoExtRand);
        strcat(filenameRand, ".txt");
    } while (f_stat(filenameRand, &root_info) != FR_NO_FILE);

    char *json_string = Preset_Save(presets[currentPreset].name);

    retSD = f_open(&SDFile, filenameRand, FA_WRITE | FA_CREATE_ALWAYS);
    retSD = f_write(&SDFile, json_string, strlen(json_string), &bw);

    if (strlen(json_string) != bw || retSD != FR_OK) {
        //TODO: Error handling
    }

    retSD = f_close(&SDFile);
    retSD = f_mount(NULL, "", 0);

    free(json_string);
    */

    return true;
}

void getRandomFileNameNoExt(char *fileNameBuf) {
    char randBuf[NUM_FN_RANDOM_CHARS + 1];
    char *currFilename = presets[currentPreset].filename;
    char fileNameNoExt[MAX_FILENAME_LENGTH - NUM_EXTENSION_CHARS + 1];

    snprintf(fileNameNoExt, strlen(currFilename), "%s", currFilename);
    strtok(fileNameNoExt, ".");

    getRandomString(randBuf);

    uint16_t len_filenameNoExt = strlen(fileNameNoExt) + strlen(randBuf) + 2;
    snprintf(fileNameBuf, strlen(fileNameNoExt) + 1, "%s", fileNameNoExt);
    strcat(fileNameBuf, "_");
    strcat(fileNameBuf, randBuf);
    fileNameBuf[len_filenameNoExt - 1] = '\0';
}

void getRandomString(char *randBuf) {
    static const char alphanum[] =
            "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < NUM_FN_RANDOM_CHARS; ++i) {
        randBuf[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    randBuf[NUM_FN_RANDOM_CHARS] = '\0';
}

