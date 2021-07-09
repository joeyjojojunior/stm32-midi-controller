/*
 * knob.c
 *
 */
#include "knob.h"

Knob knobs[MAX_KNOBS];

void Knob_Init() {
    // TODO: Change when ADC muxes set up as they should be
    knobs[0].row = 0;
    knobs[0].col = 0;
    knobs[1].row = 0;
    knobs[1].col = 1;
    knobs[2].row = 1;
    knobs[2].col = 0;
    knobs[3].row = 1;
    knobs[3].col = 1;
}

void Knob_Free(Knob *k) {
    free(k->sub_labels);
}

// Maps values from (0, max_values-1) to (0, m)
uint8_t Knob_Map(Knob *k, uint8_t value, uint8_t m) {
    return value * m / (k->max_values - 1);
}
