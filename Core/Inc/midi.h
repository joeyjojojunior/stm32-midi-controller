/*
 * midi.h
 */
#include "knob.h"

#ifndef INC_MIDI_H_
#define INC_MIDI_H_

#define EMA_A 0.5
#define UPPER_BOUND_ADC 250

void MIDI_Send(Knob *k, uint8_t value);
uint8_t MIDI_Scale_And_Filter(Knob *k, uint8_t adc_value);

#endif /* INC_MIDI_H_ */
