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
	char (*sub_labels)[MAX_LABEL_CHARS+1];
	uint8_t channel;
	uint8_t cc;
	uint8_t value;
	uint8_t max_values;
	uint8_t max_range;
} Knob;

static Knob knobs[4] = {
		{ .row = 0, .col = 0, .label = "Cutoff", .sub_label = "Filter 1", .channel = 0, .cc = 17, .value = 0, .max_values = 127, .max_range = 127 },
		{ .row = 0, .col = 1, .label = "Resonance", .sub_label = "Filter 2", .channel = 1, .cc = 18, .value = 0, .max_values = 127, .max_range = 127 },
		{ .row = 1, .col = 0, .label = "Filter Env", .sub_label = "", .channel = 2, .cc = 19, .value = 0, .max_values = 127, .max_range = 127 },
		{ .row = 1, .col = 1, .label = "Osc 1", .sub_label = "Velocity", .channel = 3, .cc = 20, .value = 0, .max_values = 3, .max_range = 127 }
};


#endif /* INC_KNOB_H_ */
