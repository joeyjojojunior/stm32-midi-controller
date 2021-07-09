#include "preset.h"
#include "ffconf.h"

Preset presets[MAX_PRESETS];
uint16_t numPresets = 0;
uint16_t currentPreset = 0;

void Preset_GetName(char *filename, char *file_buffer, char *name_buffer) {
    cJSON *preset_json = cJSON_Parse(file_buffer);
    if (preset_json == NULL) return;
    const cJSON *name = cJSON_GetObjectItemCaseSensitive(preset_json, "name");
    const cJSON *index = cJSON_GetObjectItemCaseSensitive(preset_json, "index");

    uint16_t preset_index = atoi(index->valuestring);
    snprintf(presets[preset_index].name, strlen(name->valuestring) + 1, "%s", name->valuestring);
    snprintf(presets[preset_index].filename, _MAX_LFN + 1, "%s", filename);
    presets[preset_index].index = preset_index;

    numPresets++;

    cJSON_Delete(preset_json);
}

void Preset_Load(char *buffer) {
    cJSON *preset_json = cJSON_Parse(buffer);
    if (preset_json == NULL) return;

    const cJSON *index = cJSON_GetObjectItemCaseSensitive(preset_json, "index");
    currentPreset = atoi(index->valuestring);

    const cJSON *knobs_json = cJSON_GetObjectItemCaseSensitive(preset_json, "knobs");
    const cJSON *knob_json = NULL;

    uint8_t i = 0;
    uint8_t curr_page = knobPage;
    uint8_t num_pages = cJSON_GetArraySize(knobs_json) / NUM_KNOBS;

    if (num_pages > NUM_PAGES) return;

    cJSON_ArrayForEach(knob_json, knobs_json)
    {
        if (i == NUM_KNOBS) {
            i = 0;
            knobPage = knobPage + 1;
        }

        if (knobPage >= NUM_PAGES) goto end;

        const cJSON *row = cJSON_GetObjectItemCaseSensitive(knob_json, "row");
        const cJSON *col = cJSON_GetObjectItemCaseSensitive(knob_json, "col");
        const cJSON *label = cJSON_GetObjectItemCaseSensitive(knob_json, "label");
        const cJSON *channel = cJSON_GetObjectItemCaseSensitive(knob_json, "channel");
        const cJSON *cc = cJSON_GetObjectItemCaseSensitive(knob_json, "cc");
        const cJSON *init_value = cJSON_GetObjectItemCaseSensitive(knob_json, "init_value");
        const cJSON *max_values = cJSON_GetObjectItemCaseSensitive(knob_json, "max_values");
        const cJSON *max_range = cJSON_GetObjectItemCaseSensitive(knob_json, "max_range");
        const cJSON *isLocked = cJSON_GetObjectItemCaseSensitive(knob_json, "isLocked");

        knobs[Knob_Index(i)].row = atoi(row->valuestring);
        knobs[Knob_Index(i)].col = atoi(col->valuestring);
        snprintf(knobs[Knob_Index(i)].label, sizeof(knobs[Knob_Index(i)].label) / sizeof(knobs[Knob_Index(i)].label[0]), "%s", label->valuestring);
        knobs[Knob_Index(i)].channel = atoi(channel->valuestring);
        knobs[Knob_Index(i)].cc = atoi(cc->valuestring);
        knobs[Knob_Index(i)].init_value = atoi(init_value->valuestring);
        knobs[Knob_Index(i)].value = atoi(init_value->valuestring);
        knobs[Knob_Index(i)].lock_value = atoi(init_value->valuestring);
        knobs[Knob_Index(i)].max_values = atoi(max_values->valuestring);
        knobs[Knob_Index(i)].max_range = atoi(max_range->valuestring);
        knobs[Knob_Index(i)].isLocked = atoi(isLocked->valuestring);

        const cJSON *sub_labels = cJSON_GetObjectItemCaseSensitive(knob_json, "sub_labels");
        const cJSON *sub_label = NULL;

        knobs[Knob_Index(i)].num_sl = cJSON_GetArraySize(sub_labels);
        Knob_Free(&knobs[Knob_Index(i)]);
        knobs[Knob_Index(i)].sub_labels = malloc(sizeof(*knobs[Knob_Index(i)].sub_labels) * knobs[Knob_Index(i)].num_sl);

        uint8_t label_index = 0;
        cJSON_ArrayForEach(sub_label, sub_labels)
        {
            snprintf(knobs[Knob_Index(i)].sub_labels[label_index], sizeof(knobs[Knob_Index(i)].sub_labels[0]) / sizeof(knobs[Knob_Index(i)].sub_labels[0][0]), "%s", sub_label->valuestring);
            label_index++;
        }
        i++;
    }


    end:
    knobPage = curr_page;
    cJSON_Delete(preset_json);
}

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
