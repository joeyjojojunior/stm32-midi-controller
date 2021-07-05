/*
 * sd.c
 */
#include "stm32f4xx_hal.h"
#include "sd.h"

void SD_FetchPresets() {
    //SD_Enable();

    DIR root;
    FILINFO root_info;
    retSD = f_mount(&SDFatFS, "", 1);

    retSD = f_findfirst(&root, &root_info, "", "*.json");

    /*
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(root_info.fname, Font_7x10, White);

    char ret[12];
    snprintf(ret, 12, "%d", retSD);
    ssd1306_SetCursor(0, 32);
    ssd1306_WriteString("fname: ", Font_7x10, White);
    ssd1306_SetCursor(50, 32);
    ssd1306_WriteString(ret, Font_7x10, White);


    ssd1306_UpdateScreen();
 */
    uint8_t i = 0;
    while (retSD == FR_OK && root_info.fname[0]) {
        if (i > NUM_KNOBS) break;
        retSD = f_open(&SDFile, root_info.fname, FA_READ);

        char presetBuffer[f_size(&SDFile) + 1];
        char nameBuffer[MAX_LABEL_CHARS + 1];
        unsigned int bytesRead;
        retSD = f_read(&SDFile, presetBuffer, sizeof(presetBuffer) - 1, &bytesRead);
        presetBuffer[bytesRead] = '\0';

        Preset_GetName(presetBuffer, nameBuffer);

        snprintf(presets[i], sizeof(presets[0]) / sizeof(presets[0][0]), "%s", nameBuffer);
        retSD = f_close(&SDFile);

        retSD = f_findnext(&root, &root_info);
        i++;
    }

    retSD = f_mount(NULL, "", 0);

    //SD_Disable();
}

void SD_LoadPreset(Knob *knobs, char *filename) {
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

