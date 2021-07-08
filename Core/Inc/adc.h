/*
 * adc.h
 */
#include <stdint.h>
#include "main.h"
#include "knob.h"

#ifndef INC_ADC_H_
#define INC_ADC_H_

#define NUM_ADC_SAMPLES 32
#define NUM_ADC_CHANNELS 4
#define GPIO_PORT_AMUX GPIOB

extern ADC_HandleTypeDef hadc1;
extern uint16_t adcAveraged[NUM_ADC_CHANNELS];
extern uint16_t adcAveragedPrev[NUM_KNOBS];

void ADC_ReadKnobs();
void ADC_MuxSelect(uint8_t c);

#endif /* INC_ADC_H_ */
