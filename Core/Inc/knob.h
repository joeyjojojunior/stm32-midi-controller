/*
 * knob.h
 */

#ifndef INC_KNOB_H_
#define INC_KNOB_H_

#define MAX_LABEL_CHARS 14

typedef struct Knob {
	uint8_t row;
	uint8_t col;
	char label[MAX_LABEL_CHARS + 1];
	char sub_label[MAX_LABEL_CHARS + 1];
	uint8_t channel;
	uint8_t cc;
	uint8_t value;
	uint8_t max_value;
} Knob;

static Knob knobs[4] = {
		{ .row = 0, .col = 0, .label = "Cutoff", .sub_label = "Filter 1", .channel = 1, .cc = 17, .value = 0, .max_value = 127 },
		{ .row = 0, .col = 1, .label = "Resonance", .sub_label = "Filter 2", .channel = 1, .cc = 18, .value = 0, .max_value = 127 },
		{ .row = 1, .col = 0, .label = "Osc 1", .sub_label = "SAW", .channel = 1, .cc = 19, .value = 0, .max_value = 127 },
		{ .row = 1, .col = 1, .label = "Env 1 Decay", .sub_label = "Velocity", .channel = 1, .cc = 20, .value = 0, .max_value = 127 }
};

#endif /* INC_KNOB_H_ */