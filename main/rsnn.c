#include "rsnn.h"
#include <stdlib.h> // For random numbers
#include "freertos/FreeRTOS.h" // For delay function
#include "freertos/task.h" // For delay function
#include "esp_log.h" // For logging

static const char *TAG = "DEPLOY-SYNAPTIC_STORKLIKE";

// Function to reset synaptic and membrane potentials
void reset_mem(float *syn, float *mem) {
    *syn = 0.0;
    *mem = 0.0;
}

// Function to simulate the Synaptic neuron model
void synaptic_forward(unsigned short int input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold) {
    // Update synaptic current
    *syn = *alpha * (*syn) + input;

    // Update membrane potential
    *mem = *beta * (*mem) + (*syn);

    // Generate spike if membrane potential exceeds threshold
    if (*spk == 1.0) {
        *mem -= *threshold; // Reset by subtraction
    }
    
    if (*mem > *threshold) {
        *spk = 1.0;
    } else {
        *spk = 0.0;
    }
}

// Function to simulate the RSynaptic neuron model
void rsynaptic_forward(unsigned short int input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold, const float *wrec) {
    // Update synaptic current
    *syn = *alpha * (*syn) + input + (*wrec) * (*spk);

    // Update membrane potential
    *mem = *beta * (*mem) + (*syn);

    // Generate spike if membrane potential exceeds threshold
    if (*spk == 1.0) {
        *mem -= *threshold; // Reset by subtraction
    }
    
    if (*mem > *threshold) {
        *spk = 1.0;
    } else {
        *spk = 0.0;
    }
}

// Function to simulate the Synaptic_storklike neuron model with array input and output
void synaptic_storklike_forward(unsigned short int *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold, int size) {
    for (int i = 0; i < size; i++) {
        // Update synaptic current
        float prev_syn = syn[i];
        syn[i] = alpha[i] * syn[i] + input[i];

        // Update membrane potential
        float prev_mem = mem[i];
        mem[i] = beta[i] * mem[i] + (1 - beta[i]) * prev_syn; // scl_mem = 1-dcy_mem = 1-beta

        // Generate spike if membrane potential exceeds threshold
        if (prev_mem > threshold[i]) {
            spk[i] = 1.0;
        } else {
            spk[i] = 0.0;
        }

        if (spk[i] == 1.0) {
            mem[i] -= threshold[i]; // Reset by subtraction
        }
    }
}

// Function to simulate the RSynaptic_storklike neuron model with array input and output
void rsynaptic_storklike_forward(unsigned short int *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold, const float *wrec, int size) {
    for (int i = 0; i < size; i++) {
        // Update synaptic current using previous syn
        float prev_syn = syn[i];
        syn[i] = alpha[i] * syn[i] + input[i] + wrec[i] * spk[i];

        // Update membrane potential using previous syn
        float prev_mem = mem[i];
        mem[i] = beta[i] * mem[i] + (1 - beta[i]) * prev_syn; // scl_mem = 1-dcy_mem = 1-beta

        // Generate spike if membrane potential exceeds threshold
        if (prev_mem > threshold[i]) {
            spk[i] = 1.0;
        } else {
            spk[i] = 0.0;
        }

        if (spk[i] == 1.0) {
            mem[i] -= threshold[i]; // Reset by subtraction
        }
    }
}

// Main application function
void app_main(void)
{
    ESP_LOGI(TAG, "Initializing RSNN...");

    unsigned short int input_z[T][1][INPUT_NEURONS_NUM] = {
        {{0,0,0}},
        {{3,3,3}},
        {{3,3,3}},
        {{3,3,3}},
        {{3,3,3}},
        {{0,0,0}},
        {{0,0,0}},
        {{0,0,0}},
        {{0,0,0}},
        {{0,0,0}},
        {{0,0,0}}};

    const float alpha[INPUT_NEURONS_NUM] = {0.6703, 0.6703, 0.6703}; //dcy_syn
    const float beta[INPUT_NEURONS_NUM] = {0.8187, 0.8187, 0.8187}; //dcy_mem
    const float threshold[INPUT_NEURONS_NUM] = {1.0, 1.0, 1.0};
    float syn[INPUT_NEURONS_NUM] = {0.0, 0.0, 0.0};
    float mem[INPUT_NEURONS_NUM] = {0.0, 0.0, 0.0};
    float spk[INPUT_NEURONS_NUM] = {0.0, 0.0, 0.0};
    const float wrec[INPUT_NEURONS_NUM] = {1.0, 1.0, 1.0}; // Example recurrent weights

    for(uint16_t i = 0; i < T; ++i) // for each timestep
    {
        ESP_LOGI(TAG, "  t: %d", i);
        ESP_LOGI(TAG, "  x: %d, %d, %d", input_z[i][0][0], input_z[i][0][1], input_z[i][0][2]);
        
        // Update Synaptic
        // synaptic_forward(input_z[i][0], &syn, &mem, &alpha, &beta, &threshold, &spk);

        // Update RSynaptic
        // rsynaptic_forward(input_z[i][0], &syn, &mem, &spk, &alpha, &beta, &threshold, &wrec);

        // Update Synaptic_storklike
        // synaptic_storklike_forward(input_z[i][0], syn, mem, spk, alpha, beta, threshold, INPUT_NEURONS_NUM);

        // Update RSynaptic_storklike
        rsynaptic_storklike_forward(input_z[i][0], syn, mem, spk, alpha, beta, threshold, wrec, INPUT_NEURONS_NUM);

        // Print output
        ESP_LOGI(TAG, "syn: %f, %f, %f", syn[0], syn[1], syn[2]);
        ESP_LOGI(TAG, "mem: %f, %f, %f", mem[0], mem[1], mem[2]);
        ESP_LOGI(TAG, "spk: %f, %f, %f", spk[0], spk[1], spk[2]);
        ESP_LOGI(TAG, "----------------------");
    }
}
