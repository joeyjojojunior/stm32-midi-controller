/*
 * knob.c
 *
 */
#include "knob.h"
#include "main.h"

Knob knobs[NUM_KNOBS * NUM_PAGES];
uint8_t knobPage = 0;

void Knob_Init() {
    // TODO: Change when ADC muxes set up as they should be
    for (uint8_t p = 0; p < NUM_PAGES; p++) {
        for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++) {
            knobs[i + p * NUM_KNOBS].row = i;
            knobs[i + p * NUM_KNOBS].col = 0;
        }
    }
}

void Knob_Free(Knob *k) {
    free(k->sub_labels);
}

uint16_t Knob_Index(uint8_t i) {
    return i + knobPage * NUM_KNOBS;
}

// Maps values from (0, max_values-1) to (0, m)
uint8_t Knob_Map(Knob *k, uint8_t value, uint8_t m) {
    return value * m / (k->max_values - 1);
}
