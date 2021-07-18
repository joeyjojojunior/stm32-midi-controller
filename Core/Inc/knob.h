/*
 * knob.h
 */
#include <stdint.h>
#include <stdlib.h>

#ifndef INC_KNOB_H_
#define INC_KNOB_H_

#define NUM_KNOBS 64
#define NUM_PAGES 2
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
    uint8_t lock_value;
} Knob;

extern Knob knobs[NUM_KNOBS * NUM_PAGES];
extern uint8_t knobPage;

void Knob_Init();
void Knob_LockAll();
uint16_t Knob_Index(uint8_t i);
uint8_t Knob_Map(Knob *k, uint8_t value, uint8_t m);
void Knob_Free(Knob *k);

#endif /* INC_KNOB_H_ */
