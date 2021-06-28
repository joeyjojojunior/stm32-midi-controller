/*
 * knob.c
 *
 */
#include "knob.h"

uint8_t Knob_Map(Knob k) {
	return k.value*(k.max_range/(k.max_values-1));
}

