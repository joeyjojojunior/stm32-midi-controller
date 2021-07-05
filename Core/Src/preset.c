#include "preset.h"

void Preset_GetName(char *file_buffer, char *name_buffer) {
    cJSON *preset_json = cJSON_Parse(file_buffer);
    if (preset_json == NULL) return;
    const cJSON *name = cJSON_GetObjectItemCaseSensitive(preset_json, "name");
    snprintf(name_buffer, MAX_LABEL_CHARS, "%s", name->valuestring);
}

void Preset_Load(Knob *knobs, char *buffer) {
    cJSON *preset_json = cJSON_Parse(buffer);
    if (preset_json == NULL) return;

    const cJSON *knobs_json = cJSON_GetObjectItemCaseSensitive(preset_json, "knobs");
    const cJSON *knob_json = NULL;

    uint8_t i = 0;
    cJSON_ArrayForEach(knob_json, knobs_json)
    {
        if (i >= NUM_KNOBS) return;

        const cJSON *row = cJSON_GetObjectItemCaseSensitive(knob_json, "row");
        const cJSON *col = cJSON_GetObjectItemCaseSensitive(knob_json, "col");
        const cJSON *label = cJSON_GetObjectItemCaseSensitive(knob_json, "label");
        const cJSON *channel = cJSON_GetObjectItemCaseSensitive(knob_json, "channel");
        const cJSON *cc = cJSON_GetObjectItemCaseSensitive(knob_json, "cc");
        const cJSON *init_value = cJSON_GetObjectItemCaseSensitive(knob_json, "init_value");
        const cJSON *max_values = cJSON_GetObjectItemCaseSensitive(knob_json, "max_values");
        const cJSON *max_range = cJSON_GetObjectItemCaseSensitive(knob_json, "max_range");
        const cJSON *isLocked = cJSON_GetObjectItemCaseSensitive(knob_json, "isLocked");

        knobs[i].row = row->valueint;
        knobs[i].col = col->valueint;
        snprintf(knobs[i].label, sizeof(knobs[i].label) / sizeof(knobs[i].label[0]), "%s", label->valuestring);
        knobs[i].channel = channel->valueint;
        knobs[i].cc = cc->valueint;
        knobs[i].init_value = init_value->valueint;
        knobs[i].max_values = max_values->valueint;
        knobs[i].max_range = max_range->valueint;
        knobs[i].isLocked = isLocked->valueint;

        const cJSON *sub_labels = cJSON_GetObjectItemCaseSensitive(knob_json, "sub_labels");
        const cJSON *sub_label = NULL;
        knobs[i].sub_labels = malloc(sizeof(*knobs[i].sub_labels) * (knobs[i].max_values));

        uint8_t label_index = 0;
        cJSON_ArrayForEach(sub_label, sub_labels)
        {
            snprintf(knobs[i].sub_labels[label_index], sizeof(knobs[i].sub_labels[0]) / sizeof(knobs[i].sub_labels[0][0]), "%s", sub_label->valuestring);
            label_index++;
        }
        i++;
    }
}
