#include "preset.h"

void Load_Preset(Knob *k, char* filename) {
    char* json_str = File_To_String(filename);
    cJSON* knobs_json = cJSON_Parse(json_str);
    if (knobs_json == NULL) return;

    const cJSON* knob = NULL;
    uint8_t i = 0;
    cJSON_ArrayForEach(knob, knobs_json) {
        if (i >= 4) return;

        const cJSON *row = cJSON_GetObjectItemCaseSensitive(knob, "row");
        const cJSON *col = cJSON_GetObjectItemCaseSensitive(knob, "col");
        const cJSON *label = cJSON_GetObjectItemCaseSensitive(knob, "label");
        const cJSON *channel = cJSON_GetObjectItemCaseSensitive(knob, "channel");
        const cJSON *cc = cJSON_GetObjectItemCaseSensitive(knob, "cc");
        const cJSON *init_value = cJSON_GetObjectItemCaseSensitive(knob, "init_value");
        const cJSON *max_values = cJSON_GetObjectItemCaseSensitive(knob, "max_values");
        const cJSON *max_range = cJSON_GetObjectItemCaseSensitive(knob, "max_range");
        const cJSON *isLocked = cJSON_GetObjectItemCaseSensitive(knob, "isLocked");

        k[i].row = row->valueint;
        k[i].col = col->valueint;
        snprintf(k[i].label, sizeof(k[i].label)/sizeof(k[i].label[0]), "%s", label->valuestring);
        k[i].channel = channel->valueint;
        k[i].cc = cc->valueint;
        k[i].init_value = init_value->valueint;
        k[i].max_values = max_values->valueint;
        k[i].max_range = max_range->valueint;
        k[i].isLocked = isLocked->valueint;

        const cJSON* sub_labels = cJSON_GetObjectItemCaseSensitive(knob, "sub_labels");
        const cJSON* sub_label = NULL;
        k[i].sub_labels = malloc(sizeof(*k[i].sub_labels) * (k[i].max_values));

        uint8_t label_index = 0;
        cJSON_ArrayForEach(sub_label, sub_labels) {
            snprintf(k[i].sub_labels[label_index], sizeof(k[i].sub_labels[0]) / sizeof(k[i].sub_labels[0][0]), "%s", sub_label->valuestring);
            label_index++;
        }
        i++;
    }
}

void Print_Knob(Knob k) {
    printf("row: %d\n", k.row);
    printf("col: %d\n", k.col);
    printf("label: %s\n", k.label);
    printf("sub_labels:\n");

    if (k.max_values == 128) {
        printf("    %s\n", k.sub_labels[0]);
    }
    else {
        for (uint8_t i = 0; i < k.max_values; i++) {
            printf("    %s\n", k.sub_labels[i]);
        }
    }

    printf("channel: %d\n", k.channel);
    printf("cc: %d\n", k.cc);
    printf("init_value: %d\n", k.init_value);
    printf("max_values: %d\n", k.max_values);
    printf("max_range: %d\n", k.max_range);
    printf("isLocked: %d\n", k.isLocked);
    printf("\n");
}

char * File_To_String(char* filename) {
    char* buffer = 0;
    long length = 0;
    FILE* f;

    f = fopen(filename, "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }

    if (buffer)
    {
        buffer[length] = '\0';
    }

    return buffer;
}
