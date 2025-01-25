#include "rsnn.h"
#include <stdlib.h> // For random numbers
#include "freertos/FreeRTOS.h" // For delay function
#include "freertos/task.h" // For delay function
#include "esp_log.h" // For logging
#include <string.h> // For memcpy function

static const char *TAG = "ESP32S3";

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
void synaptic_storklike_forward(float *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold, int size) {
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
void rsynaptic_storklike_forward(float *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold, const float wrec[INPUT_NEURONS_NUM][INPUT_NEURONS_NUM], int size) {
    float prev_spk[INPUT_NEURONS_NUM];
    memcpy(prev_spk, spk, size * sizeof(spk[0])); // Save spk array as prev_spk

    for (int i = 0; i < size; i++) { // loop through neuron input
        // Update synaptic current using previous syn and all-to-all connections
        float prev_syn = syn[i];
        syn[i] = alpha[i] * syn[i] + input[i];
        for (int j = 0; j < size; j++) { // loop through neuron recurrent input
            syn[i] += wrec[i][j] * prev_spk[j];
        }

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

    // float input_z[T][1][INPUT_NEURONS_NUM] = {
    //     {{0}},
    //     {{1}},
    //     {{1}},
    //     {{1}},
    //     {{1}},
    //     {{0}},
    //     {{0}},
    //     {{0}},
    //     {{0}},
    //     {{0}},
    //     {{0}}};

    // float input_z[T][1][INPUT_NEURONS_NUM] = {
    //     {{0,0,0}},
    //     {{1,1,1}},
    //     {{1,1,1}},
    //     {{1,1,1}},
    //     {{1,1,1}},
    //     {{0,0,0}},
    //     {{0,0,0}},
    //     {{0,0,0}},
    //     {{0,0,0}},
    //     {{0,0,0}},
    //     {{0,0,0}}};

    float input_z[T][1][INPUT_NEURONS_NUM] = {
        {{0,0,0}},
        {{1,1,0}},
        {{1,0,0}},
        {{1,0,0}},
        {{1,0,0}},
        {{0,1,1}},
        {{0,0,1}},
        {{0,0,1}},
        {{0,0,1}},
        {{0,1,0}},
        {{0,0,0}}};

    const float alpha1[INPUT_NEURONS_NUM] = {0.6703, 0.6703, 0.6703}; //dcy_syn
    const float beta1[INPUT_NEURONS_NUM] = {0.8187, 0.8187, 0.8187}; //dcy_mem
    const float threshold1[INPUT_NEURONS_NUM] = {1.0, 1.0, 1.0};
    const float alpha2[INPUT_NEURONS_NUM] = {0.6703, 0.6703, 0.6703}; //dcy_syn
    const float beta2[INPUT_NEURONS_NUM] = {0.8187, 0.8187, 0.8187}; //dcy_mem
    const float threshold2[INPUT_NEURONS_NUM] = {1.0, 1.0, 1.0};
    float syn1[INPUT_NEURONS_NUM] = {0.0};
    float mem1[INPUT_NEURONS_NUM] = {0.0};
    float spk1[INPUT_NEURONS_NUM] = {0.0};
    float syn2[INPUT_NEURONS_NUM] = {0.0};
    float mem2[INPUT_NEURONS_NUM] = {0.0};
    float spk2[INPUT_NEURONS_NUM] = {0.0};
    // const float wrec[INPUT_NEURONS_NUM][INPUT_NEURONS_NUM] = {
    //     {1.0}
    // }; // Example recurrent weights

    const float wrec[INPUT_NEURONS_NUM][INPUT_NEURONS_NUM] = {
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0}
    }; // Example recurrent weights

    for(uint16_t i = 0; i < T; ++i) // for each timestep
    {
        ESP_LOGI(TAG, "   t: %d", i);
        // ESP_LOGI(TAG, "  x: %d, %d, %d", input_z[i][0][0], input_z[i][0][1], input_z[i][0][2]);
        ESP_LOGI(TAG, "   x: %f, %f, %f", input_z[i][0][0], input_z[i][0][1], input_z[i][0][2]);
        
        // Update Synaptic
        // synaptic_forward(input_z[i][0], &syn, &mem, &alpha, &beta, &threshold, &spk);

        // Update RSynaptic
        // rsynaptic_forward(input_z[i][0], &syn, &mem, &spk, &alpha, &beta, &threshold, &wrec);

        // Update Synaptic_storklike
        // synaptic_storklike_forward(input_z[i][0], syn, mem, spk, alpha, beta, threshold, INPUT_NEURONS_NUM);

        // Update RSynaptic_storklike
        rsynaptic_storklike_forward(input_z[i][0], syn1, mem1, spk1, alpha1, beta1, threshold1, wrec, INPUT_NEURONS_NUM);

        // Print output
        ESP_LOGI(TAG, "syn1: %f, %f, %f", syn1[0], syn1[1], syn1[2]);
        ESP_LOGI(TAG, "mem1: %f, %f, %f", mem1[0], mem1[1], mem1[2]);
        ESP_LOGI(TAG, "spk1: %f, %f, %f", spk1[0], spk1[1], spk1[2]);

        // Update Synaptic_storklike
        synaptic_storklike_forward(spk1, syn2, mem2, spk2, alpha2, beta2, threshold2, INPUT_NEURONS_NUM);

        // Print output
        ESP_LOGI(TAG, "syn2: %f, %f, %f", syn2[0], syn2[1], syn2[2]);
        ESP_LOGI(TAG, "mem2: %f, %f, %f", mem2[0], mem2[1], mem2[2]);
        ESP_LOGI(TAG, "spk2: %f, %f, %f", spk2[0], spk2[1], spk2[2]);
        ESP_LOGI(TAG, "----------------------");
    }
}
