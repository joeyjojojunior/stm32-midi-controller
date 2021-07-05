/*
 * sd.c
 */
#include "stm32f4xx_hal.h"
#include "sd.h"
#include "fatfs.h"
#include "preset.h"

void SD_LoadPreset(Knob *knobs, char* filename) {
    retSD = f_mount(&SDFatFS, "", 1);
    retSD = f_open(&SDFile, filename, FA_READ);

    char presetBuffer[f_size(&SDFile) + 1];
    unsigned int bytesRead;

    retSD = f_read(&SDFile, presetBuffer, sizeof(presetBuffer) - 1, &bytesRead);
    presetBuffer[bytesRead] = '\0';

    Preset_Load(knobs, presetBuffer);

    retSD = f_close(&SDFile);
    retSD = f_mount(NULL, "", 0);

    SD_Disable(hsd);
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

