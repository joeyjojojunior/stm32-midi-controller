/*
 * knob.c
 *
 */
#include "knob.h"
#include "main.h"

Knob knobs[NUM_KNOBS * NUM_PAGES];
uint8_t knobPage = 0;

void Knob_Init() {
    for (uint8_t p = 0; p < NUM_PAGES; p++) {
        for (uint8_t col = 0; col < NUM_COLS; col++) {
            for (uint8_t row = 0; row < NUM_ROWS; row++) {
                uint8_t i = NUM_COLS * row + col;
                knobs[i + p * NUM_KNOBS].row = row;
                knobs[i + p * NUM_KNOBS].col = col;
            }
        }
    }
    // TODO: Change when ADC muxes set up as they should be
    /*
    for (uint8_t p = 0; p < NUM_PAGES; p++) {
        for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++) {
            knobs[i + p * NUM_KNOBS].row = i;
            knobs[i + p * NUM_KNOBS].col = 0;
        }
    }
    */
}

// Converts a knob index from 0 to NUM_KNOBS to select a knob page
uint16_t Knob_Index(uint8_t i) {
    return i + knobPage * NUM_KNOBS;
}

void Knob_LockAll() {
    for (uint16_t i = 0; i < NUM_KNOBS; i++) {
        knobs[Knob_Index(i)].lock_value = knobs[Knob_Index(i)].value;
        knobs[Knob_Index(i)].isLocked = 1;
    }
}

// Maps values from (0, max_values-1) to (0, m)
uint8_t Knob_Map(Knob *k, uint8_t value, uint8_t m) {
    return value * m / (k->max_values - 1);
}

void Knob_Free(Knob *k) {
    free(k->sub_labels);
}
