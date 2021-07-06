/*
 * midi.c
 */

#include "midi.h"
#include "usb_device.h"

// Sends a CC message for knob k with the specified value
void MIDI_Send(Knob *k, uint8_t value) {
    MX_USB_Send_Midi(k->channel, k->cc, Knob_Map(k, value, k->max_range));
}

// Scales an ADC value from (0, 255) to (0, k->max_values) an applies EMA filter
uint8_t MIDI_Scale_And_Filter(Knob *k, uint8_t adc_value) {
    float midi_scale_factor = 1.0 * k->max_values / UPPER_BOUND_ADC;
    return MIN(EMA_A * midi_scale_factor * adc_value + (1 - EMA_A) * k->value, k->max_range);
}

uint8_t MIDI_ScaleMax_And_Filter(Knob *k, uint8_t adc_value) {
    float midi_scale_factor = 1.0 * MIDI_MAX / UPPER_BOUND_ADC;
    return MIN(EMA_A * midi_scale_factor * adc_value + (1 - EMA_A) * k->value, k->max_range);
}
