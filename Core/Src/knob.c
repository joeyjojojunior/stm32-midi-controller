/*
 * knob.c
 *
 */
#include "knob.h"

// Maps values from (0, max_values-1) to (0, max_range)
//uint8_t Knob_Map(Knob *k) {
//	return k->value * k->max_range / (k->max_values - 1);
//}

// Maps values from (0, max_values-1) to (0, max)
uint8_t KnobMap(Knob *k, uint8_t value, uint8_t m) {
	return value * m / (k->max_values - 1);
}
