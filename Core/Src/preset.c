#include "preset.h"
#include "ffconf.h"
#include <stdbool.h>

Preset presets[MAX_PRESETS];
uint16_t numPresets = 0;
uint16_t currentPreset = 0;
char tokens[MAX_KNOB_TOKENS][MAX_KNOB_LABEL_CHARS + 1];


/*
void Preset_GetName(char *filename, char *file_buffer, char *name_buffer) {
    cJSON *preset_json = cJSON_Parse(file_buffer);
    if (preset_json == NULL) return;
    const cJSON *name = cJSON_GetObjectItemCaseSensitive(preset_json, "name");
    const cJSON *sub_label = cJSON_GetObjectItemCaseSensitive(preset_json, "sub_label");
    const cJSON *index = cJSON_GetObjectItemCaseSensitive(preset_json, "index");

    uint16_t preset_index = atoi(index->valuestring);
    snprintf(presets[preset_index].name, strlen(name->valuestring) + 1, "%s", name->valuestring);
    snprintf(presets[preset_index].sub_label, strlen(sub_label->valuestring) + 1, "%s", sub_label->valuestring);
    snprintf(presets[preset_index].filename, _MAX_LFN + 1, "%s", filename);
    presets[preset_index].index = preset_index;

    numPresets++;

    cJSON_Delete(preset_json);
}
*/

/*
void Preset_GetName(char *filename, char *file_buffer, char *name_buffer) {
    memset(&mem[0], 0, sizeof(mem));

    json_t const* json = json_create(file_buffer, mem, sizeof(mem) / sizeof(*mem));

    if (!json) {
        puts("Error json create.");
    }

    json_t const* preset_name = json_getProperty(json, "name");
    if (!preset_name || JSON_TEXT != json_getType(preset_name)) {
        puts("Error, the preset name property is not found.");
    }
    //char const* preset_name_val = json_getValue(preset_name);
    //printf("name: %s\n", preset_name_val);

    json_t const* preset_sub_label = json_getProperty(json, "sub_label");
    if (!preset_sub_label || JSON_TEXT != json_getType(preset_sub_label)) {
        puts("Error, the preset sub label property is not found.");
    }
    //char const* preset_sub_label_val = json_getValue(preset_sub_label);
    //printf("sub_label: %s\n", preset_sub_label_val);

    json_t const* index = json_getProperty(json, "index");
    if (!index || JSON_TEXT != json_getType(index)) {
        puts("Error, the preset index property is not found.");
    }
    //char const* index_val = json_getValue(index);
    //printf("index: %s\n", index_val);

    uint16_t preset_index = atoi(json_getValue(index));
    snprintf(presets[preset_index].name, strlen(json_getValue(preset_name)) + 1, "%s", json_getValue(preset_name));
    snprintf(presets[preset_index].sub_label, strlen(json_getValue(preset_sub_label)) + 1, "%s", json_getValue(preset_sub_label));
    snprintf(presets[preset_index].filename, _MAX_LFN + 1, "%s", filename);
    presets[preset_index].index = preset_index;

    numPresets++;
}
*/

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

    uint8_t z = 0;

    while (curLine) {
        // already processed first line in Preset_GetName

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

        // Count non-empty sub labels and allocate space for them



        knobs[Knob_Index(i)].num_sl = 0;
        for (uint8_t j = 7; j < MAX_KNOB_TOKENS; j++) {
           if (tokens[j][0] == '\0') break;
           knobs[Knob_Index(i)].num_sl++;
        }
        Knob_Free(&knobs[Knob_Index(i)]);
        knobs[Knob_Index(i)].sub_labels = malloc(sizeof(*knobs[Knob_Index(i)].sub_labels) * knobs[Knob_Index(i)].num_sl);


        // Add sub labels

        uint8_t label_index = 0;
        for (uint8_t j = 7; j < knobs[Knob_Index(i)].num_sl + 7; j++) {
            if (tokens[j][0] == '\0') break;
            snprintf(knobs[Knob_Index(i)].sub_labels[label_index], sizeof(knobs[Knob_Index(i)].sub_labels[0]) / sizeof(knobs[Knob_Index(i)].sub_labels[0][0]), "%s", tokens[j]);
            label_index++;
        }

        if (z == 93) {
            //break;
            printf(" ");
        }



        if (nextLine) *nextLine = '\n'; // then restore newline-char, just to be tidy
        curLine = nextLine ? (nextLine + 1) : NULL;

        i++;

        z++;


    }
    knobPage = curr_page;
}


/*
void Preset_Load(char *buffer) {
    memset(&mem[0], 0, sizeof(mem));

    json_t const* json = json_create(buffer, mem, sizeof(mem) / sizeof(*mem));

    if (!json) {
        puts("Error json create.");
    }

    json_t const* index = json_getProperty(json, "index");
    if (!index || JSON_TEXT != json_getType(index)) {
        puts("Error, the preset index property is not found.");
    }
    currentPreset = atoi(json_getValue(index));

    json_t const* knobs_json = json_getProperty(json, "knobs");
    if (!knobs_json || JSON_ARRAY != json_getType(knobs_json)) {
        puts("Error, the preset knobs property is not found.");
    }

    json_t const* knob;
    uint8_t i = 0;
    uint8_t curr_page = knobPage;
    uint8_t num_pages = 2;

    if (num_pages > NUM_PAGES) return;

    for (knob = json_getChild(knobs_json); knob != 0; knob = json_getSibling(knob)) {
        if (JSON_OBJ == json_getType(knob)) {

            if (i == NUM_KNOBS) {
                i = 0;
                knobPage = knobPage + 1;
            }

            if (knobPage >= NUM_PAGES) break;

            snprintf(knobs[Knob_Index(i)].label, sizeof(knobs[Knob_Index(i)].label) / sizeof(knobs[Knob_Index(i)].label[0]), "%s", json_getPropertyValue(knob, "n"));
            knobs[Knob_Index(i)].channel = atoi(json_getPropertyValue(knob, "ch"));
            knobs[Knob_Index(i)].cc = atoi(json_getPropertyValue(knob, "cc"));
            knobs[Knob_Index(i)].init_value = atoi(json_getPropertyValue(knob, "i"));
            knobs[Knob_Index(i)].value = atoi(json_getPropertyValue(knob, "i"));
            knobs[Knob_Index(i)].lock_value = atoi(json_getPropertyValue(knob, "i"));
            knobs[Knob_Index(i)].max_values = atoi(json_getPropertyValue(knob, "v"));
            knobs[Knob_Index(i)].max_range = atoi(json_getPropertyValue(knob, "r"));
            knobs[Knob_Index(i)].isLocked = atoi(json_getPropertyValue(knob, "l"));

            // Fetch sublabels
            json_t const* sub_labels = json_getProperty(knob, "s");
            if (!sub_labels || JSON_ARRAY != json_getType(sub_labels)) {
                puts("Error, current knob's sub_labels property is not found.");
            }

            json_t const* sub_label;
            knobs[Knob_Index(i)].num_sl = 0;
            for (sub_label = json_getChild(sub_labels); sub_label != 0; sub_label = json_getSibling(sub_label)) {
                knobs[Knob_Index(i)].num_sl++;
            }
            Knob_Free(&knobs[Knob_Index(i)]);
            knobs[Knob_Index(i)].sub_labels = malloc(sizeof(*knobs[Knob_Index(i)].sub_labels) * knobs[Knob_Index(i)].num_sl);

            uint8_t label_index = 0;
            for (sub_label = json_getChild(sub_labels); sub_label != 0; sub_label = json_getSibling(sub_label)) {
                char const* sub_label_value = json_getValue(sub_label);
                snprintf(knobs[Knob_Index(i)].sub_labels[label_index], sizeof(knobs[Knob_Index(i)].sub_labels[0]) / sizeof(knobs[Knob_Index(i)].sub_labels[0][0]), "%s", sub_label_value);
                label_index++;
            }

            i++;
        }
    }

    knobPage = curr_page;


}
*/

/*
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

        //const cJSON *row = cJSON_GetObjectItemCaseSensitive(knob_json, "row");
        //const cJSON *col = cJSON_GetObjectItemCaseSensitive(knob_json, "col");
        const cJSON *label = cJSON_GetObjectItemCaseSensitive(knob_json, "label");
        const cJSON *channel = cJSON_GetObjectItemCaseSensitive(knob_json, "channel");
        const cJSON *cc = cJSON_GetObjectItemCaseSensitive(knob_json, "cc");
        const cJSON *init_value = cJSON_GetObjectItemCaseSensitive(knob_json, "init_value");
        const cJSON *max_values = cJSON_GetObjectItemCaseSensitive(knob_json, "max_values");
        const cJSON *max_range = cJSON_GetObjectItemCaseSensitive(knob_json, "max_range");
        const cJSON *isLocked = cJSON_GetObjectItemCaseSensitive(knob_json, "isLocked");

        //knobs[Knob_Index(i)].row = atoi(row->valuestring);
        //knobs[Knob_Index(i)].col = atoi(col->valuestring);
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
*/

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
