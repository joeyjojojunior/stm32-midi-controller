#include "preset.h"
#include "ffconf.h"

Preset presets[MAX_PRESETS];
uint16_t numPresets = 0;
uint16_t currentPreset = 0;
char tokens[MAX_KNOB_TOKENS][MAX_KNOB_LABEL_CHARS + 1];

void Preset_GetName(char *filename, char *file_buffer, char *name_buffer) {
    memset(tokens, '\0', sizeof(char) * 3 * (MAX_KNOB_LABEL_CHARS + 1));

    char* curLine = file_buffer;
    char* nextLine = strchr(curLine, '\n');

    if (nextLine) *nextLine = '\0';
    tokenizer(curLine, ',', tokens);

    uint16_t preset_index = atoi(tokens[2]);
    snprintf(presets[preset_index].name, strlen(tokens[0]) + 1, "%s", tokens[0]);
    snprintf(presets[preset_index].sub_label, strlen(tokens[1]) + 1, "%s", tokens[1]);
    snprintf(presets[preset_index].filename, _MAX_LFN + 1, "%s", filename);
    presets[preset_index].index = preset_index;
}

void Preset_Load(char *buffer) {
    char* curLine = strchr(buffer, '\n') + 1;
    uint8_t i = 0;
    uint8_t curr_page = knobPage;

    while (curLine) {
        if (i == NUM_KNOBS) { // start new page
            i = 0;
            knobPage = knobPage + 1;
        }

        if (knobPage >= NUM_PAGES) break; // page overflow

        char* nextLine = strchr(curLine, '\n');
        if (nextLine) *nextLine = '\0'; // temporarily terminate the current line

        memset(tokens, '\0', sizeof(char) * MAX_KNOB_TOKENS * (MAX_KNOB_LABEL_CHARS + 1));
        tokenizer(curLine, ',', tokens);

        // Grab knob settings
        snprintf(knobs[Knob_Index(i)].label, sizeof(knobs[Knob_Index(i)].label) / sizeof(knobs[Knob_Index(i)].label[0]), "%s", tokens[0]);
        knobs[Knob_Index(i)].channel = atoi(tokens[1]);
        knobs[Knob_Index(i)].cc = atoi(tokens[2]);
        knobs[Knob_Index(i)].init_value = atoi(tokens[3]);
        knobs[Knob_Index(i)].value = atoi(tokens[3]);
        knobs[Knob_Index(i)].lock_value = atoi(tokens[3]);
        knobs[Knob_Index(i)].max_values = atoi(tokens[4]);
        knobs[Knob_Index(i)].max_range = atoi(tokens[5]);
        knobs[Knob_Index(i)].isLocked = atoi(tokens[6]);

        knobs[Knob_Index(i)].num_sl = atoi(tokens[7]);
        Knob_Free(&knobs[Knob_Index(i)]);
        knobs[Knob_Index(i)].sub_labels = malloc(sizeof(*knobs[Knob_Index(i)].sub_labels) * knobs[Knob_Index(i)].num_sl);

        // Add sub labels
        uint8_t label_index = 0;
        for (uint8_t j = 8; j < knobs[Knob_Index(i)].num_sl + 8; j++) {
            if (tokens[j][0] == '\0') break;
            snprintf(knobs[Knob_Index(i)].sub_labels[label_index], sizeof(knobs[Knob_Index(i)].sub_labels[0]) / sizeof(knobs[Knob_Index(i)].sub_labels[0][0]), "%s", tokens[j]);
            label_index++;
        }

        if (nextLine) *nextLine = '\n'; // then restore newline-char, just to be tidy
        curLine = nextLine ? (nextLine + 1) : NULL;

        i++;
    }
    knobPage = curr_page;
}

/*
char* Preset_Save(char *preset_name) {
    char *json_string = NULL;
    char numBuf[4];

    cJSON *name = NULL;
    cJSON *index = NULL;
    cJSON *knobs_arr = NULL;
    cJSON *knob = NULL;
    cJSON *row = NULL;
    cJSON *col = NULL;
    cJSON *label = NULL;
    cJSON *sub_labels = NULL;
    cJSON *sub_label = NULL;
    cJSON *channel = NULL;
    cJSON *cc = NULL;
    cJSON *init_value = NULL;
    cJSON *max_values = NULL;
    cJSON *max_range = NULL;
    cJSON *isLocked = NULL;

    cJSON *preset_json = cJSON_CreateObject();

    name = cJSON_CreateString(preset_name);
    cJSON_AddItemToObject(preset_json, "name", name);

    itoa(numPresets, numBuf, 10);
    index = cJSON_CreateString(numBuf);
    cJSON_AddItemToObject(preset_json, "index", index);

    knobs_arr = cJSON_CreateArray();
    cJSON_AddItemToObject(preset_json, "knobs", knobs_arr);

    for (uint8_t i = 0; i < NUM_KNOBS; i++) {
        knob = cJSON_CreateObject();
        cJSON_AddItemToArray(knobs_arr, knob);

        itoa(knobs[Knob_Index(i)].row, numBuf, 10);
        row = cJSON_CreateString(numBuf);
        cJSON_AddItemToObject(knob, "row", row);

        itoa(knobs[Knob_Index(i)].col, numBuf, 10);
        col = cJSON_CreateString(numBuf);
        cJSON_AddItemToObject(knob, "col", col);

        label = cJSON_CreateString(knobs[Knob_Index(i)].label);
        cJSON_AddItemToObject(knob, "label", label);

        sub_labels = cJSON_CreateArray();
        cJSON_AddItemToObject(knob, "sub_labels", sub_labels);

        for (uint8_t j = 0; j < knobs[Knob_Index(i)].num_sl; j++) {
            sub_label = cJSON_CreateString(knobs[Knob_Index(i)].sub_labels[j]);
            cJSON_AddItemToArray(sub_labels, sub_label);
        }

        itoa(knobs[Knob_Index(i)].channel, numBuf, 10);
        channel = cJSON_CreateString(numBuf);
        cJSON_AddItemToObject(knob, "channel", channel);

        itoa(knobs[Knob_Index(i)].cc, numBuf, 10);
        cc = cJSON_CreateString(numBuf);
        cJSON_AddItemToObject(knob, "cc", cc);

        itoa(knobs[Knob_Index(i)].init_value, numBuf, 10);
        init_value = cJSON_CreateString(numBuf);
        cJSON_AddItemToObject(knob, "init_value", init_value);

        itoa(knobs[Knob_Index(i)].max_values, numBuf, 10);
        max_values = cJSON_CreateString(numBuf);
        cJSON_AddItemToObject(knob, "max_values", max_values);

        itoa(knobs[Knob_Index(i)].max_range, numBuf, 10);
        max_range = cJSON_CreateString(numBuf);
        cJSON_AddItemToObject(knob, "max_range", max_range);

        itoa(1, numBuf, 10);
        isLocked = cJSON_CreateString(numBuf);
        cJSON_AddItemToObject(knob, "isLocked", isLocked);
    }

    json_string = cJSON_Print(preset_json);
    cJSON_Delete(preset_json);

    numPresets++;

    return json_string;
}
*/

void tokenizer(char inputStr[], char delim, char buf[][MAX_KNOB_LABEL_CHARS + 1]) {
    char* line_buf;
    line_buf = strtok(inputStr, ",");

    uint8_t i = 0;
    while (line_buf) {
        snprintf(buf[i], sizeof(buf[0]) / sizeof(buf[0][0]), "%s", line_buf);
        line_buf = strtok(NULL, ",");
        while (line_buf && *line_buf == '\040')
            line_buf++;

        i++;
    }
}
