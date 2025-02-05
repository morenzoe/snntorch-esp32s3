#pragma once

#include "freertos/FreeRTOS.h" // For delay function
#include "esp_log.h" // For logging
#include "esp_timer.h" // For timer function
#include <string.h> // For memcpy function

// Functions
void synaptic_storklike_forward(float *input, float *syn, float *mem, const float *alpha, const float *beta, const float threshold, int size);
void rsynaptic_storklike_forward(float *input, float *syn, float *mem, uint8_t *spk, const float *alpha, const float *beta, const float threshold, const float *wrec, int size);
void linear(const uint8_t *input, const float *weight, float *output, int input_size, int hidden_size);