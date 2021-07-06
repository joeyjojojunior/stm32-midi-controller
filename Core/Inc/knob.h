/*
 * knob.h
 */
#include <stdint.h>
#include <stdlib.h>

#ifndef INC_KNOB_H_
#define INC_KNOB_H_

#define MAX_LABEL_CHARS 14
#define MIDI_MAX 127
#define NUM_KNOBS 4


typedef struct Knob {
	uint8_t row;
	uint8_t col;
	char label[MAX_LABEL_CHARS + 1];
	char (*sub_labels)[MAX_LABEL_CHARS+1];
	uint8_t channel;
	uint8_t cc;
    uint8_t init_value;
	uint8_t value;
	uint8_t max_values;
	uint8_t max_range;
	uint8_t isLocked;
} Knob;

extern Knob knobs[4];

void Knob_Free(Knob *k);
uint8_t Knob_Map(Knob *k, uint8_t value, uint8_t m);

#endif /* INC_KNOB_H_ */
