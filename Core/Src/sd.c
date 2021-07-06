/*
 * sd.c
 */
#include "stm32f4xx_hal.h"
#include "sd.h"
#include "ssd1306.h"

void SD_FetchPresetNames() {
    DIR root;
    FILINFO root_info;
    retSD = f_mount(&SDFatFS, "", 1);

    uint8_t presetCount = 0;
    retSD = f_findfirst(&root, &root_info, "", "*.json");

    while (retSD == FR_OK && root_info.fname[0]) {
        presetCount++;
        retSD = f_findnext(&root, &root_info);
    }
    f_closedir(&root);

    char filenames[presetCount][_MAX_LFN+1];
    uint8_t i = 0;
    retSD = f_findfirst(&root, &root_info, "", "*.json");
    while (retSD == FR_OK && root_info.fname[0]) {
        snprintf(filenames[i], _MAX_LFN+1, "%s", root_info.fname);
        retSD = f_findnext(&root, &root_info);
        i++;
    }

    qsort(filenames, presetCount, sizeof(filenames[0]), qsort_cmp);

    for (i = 0; i < presetCount; i++) {
        retSD = f_open(&SDFile, filenames[i], FA_READ);

        char presetBuffer[f_size(&SDFile) + 1];
        char nameBuffer[MAX_LABEL_CHARS + 1];
        unsigned int bytesRead;
        retSD = f_read(&SDFile, presetBuffer, sizeof(presetBuffer) - 1, &bytesRead);
        presetBuffer[bytesRead] = '\0';

        Preset_GetName(presetBuffer, nameBuffer);

        snprintf(presets[i], MAX_LABEL_CHARS + 1, "%s", nameBuffer);
        retSD = f_close(&SDFile);
    }

    retSD = f_mount(NULL, "", 0);
}

void SD_LoadPreset(char *filename) {
    //SD_Enable();

    retSD = f_mount(&SDFatFS, "", 1);
    retSD = f_open(&SDFile, filename, FA_READ);

    char presetBuffer[f_size(&SDFile) + 1];
    unsigned int bytesRead;

    retSD = f_read(&SDFile, presetBuffer, sizeof(presetBuffer) - 1, &bytesRead);
    presetBuffer[bytesRead] = '\0';

    Preset_Load(knobs, presetBuffer);

    retSD = f_close(&SDFile);
    retSD = f_mount(NULL, "", 0);

    //SD_Disable();
}

void SD_Toggle() {
    hsd.State != HAL_SD_STATE_READY ? SD_Enable() : SD_Disable();
}

void SD_Enable() {
    __HAL_SD_ENABLE(hsd);
    hsd.State = HAL_SD_STATE_READY;
}

void SD_Disable() {
    __HAL_SD_DISABLE(hsd);
    hsd.State = HAL_SD_STATE_RESET;
}

int qsort_cmp(const void *lhs, const void *rhs) {
    return strcmp(lhs, rhs);
}

