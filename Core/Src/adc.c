/*
 * adc.c
 */

#include "adc.h"

uint16_t adcAveraged[NUM_ADC_CHANNELS] = { 0 };
uint16_t adcAveragedPrev[NUM_KNOBS] = { 0 };
const uint32_t adcChannels[NUM_ADC_CHANNELS] = { ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3 };
const uint16_t AMUXPins[4] = { AMUX_S0_Pin, AMUX_S1_Pin, AMUX_S2_Pin, AMUX_S3_Pin };

void ADC_MuxSelect(uint8_t c) {
    if (c > AMUX_MAX_BYTE) return;

    for (int i = 0; i < NUM_AMUX_SELECT_BITS; i++) {
        if (c & (1 << i)) {
            HAL_GPIO_WritePin(GPIO_PORT_AMUX, AMUXPins[i], GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIO_PORT_AMUX, AMUXPins[i], GPIO_PIN_RESET);
        }
    }
}

void ADC_ReadKnobs(uint8_t col) {
    ADC_MuxSelect(col);

    for (uint8_t channel = 0; channel < NUM_ADC_CHANNELS; channel++) {
        uint16_t adcBuf[NUM_ADC_SAMPLES];

        // Select channel
        ADC_ChannelConfTypeDef sConfig = { 0 };
        sConfig.Channel = adcChannels[channel];
        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
            Error_Handler();
        }

        // Sample the channel NUM_ADC_SAMPLES times to the buffer
        HAL_ADC_Start(&hadc1);
        for (uint8_t i = 0; i < NUM_ADC_SAMPLES; i++) {
            HAL_ADC_PollForConversion(&hadc1, 1000);
            adcBuf[i] = HAL_ADC_GetValue(&hadc1);
        }
        HAL_ADC_Stop(&hadc1);

        // Calculate average of all samples for the channel
        uint16_t adc_sum = 0;
        for (uint8_t i = 0; i < NUM_ADC_SAMPLES; i++) {
            adc_sum += adcBuf[i];
        }

        adcAveraged[channel] = adc_sum / NUM_ADC_SAMPLES;
    }
}
