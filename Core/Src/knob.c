/*
 * knob.c
 *
 */
#include "knob.h"

Knob knobs[MAX_KNOBS];

void Knob_Free(Knob *k) {
    free(k->sub_labels);
}

// Maps values from (0, max_values-1) to (0, m)
uint8_t Knob_Map(Knob *k, uint8_t value, uint8_t m) {
	return value * m / (k->max_values - 1);
}
