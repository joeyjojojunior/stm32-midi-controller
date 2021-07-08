/*
 * knob.h
 */
#include <stdint.h>
#include <stdlib.h>

#ifndef INC_KNOB_H_
#define INC_KNOB_H_

#define NUM_KNOBS 4
#define NUM_PAGES 5
#define MAX_KNOBS NUM_KNOBS * NUM_PAGES
#define MAX_KNOB_LABEL_CHARS 14
#define MIDI_MAX 127

typedef struct Knob {
    uint8_t row;
    uint8_t col;
    char label[MAX_KNOB_LABEL_CHARS + 1];
    char (*sub_labels)[MAX_KNOB_LABEL_CHARS + 1];
    uint8_t num_sl;
    uint8_t channel;
    uint8_t cc;
    uint8_t init_value;
    uint8_t value;
    uint8_t max_values;
    uint8_t max_range;
    uint8_t isLocked;
} Knob;

extern Knob knobs[MAX_KNOBS];

void Knob_Free(Knob *k);
uint8_t Knob_Map(Knob *k, uint8_t value, uint8_t m);

#endif /* INC_KNOB_H_ */
