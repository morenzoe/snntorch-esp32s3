#include "rsnn.h"
#include <stdlib.h> // For random numbers
#include "freertos/FreeRTOS.h" // For delay function
#include "freertos/task.h" // For delay function
#include "esp_log.h" // For logging

static const char *TAG = "DEPLOY-RSYNAPTIC_STORKLIKE";

// Function to reset synaptic and membrane potentials
void reset_mem(float *syn, float *mem) {
    *syn = 0.0;
    *mem = 0.0;
}

// Function to simulate the Synaptic neuron model
void synaptic_forward(float input, float *syn, float *mem, float alpha, float beta, float threshold, float *spk) {
    // Update synaptic current
    *syn = alpha * (*syn) + input;

    // Update membrane potential
    *mem = beta * (*mem) + (*syn);

    // Generate spike if membrane potential exceeds threshold
    if (*spk == 1.0) {
        *mem -= threshold; // Reset by subtraction
    }
    
    if (*mem > threshold) {
        *spk = 1.0;
    } else {
        *spk = 0.0;
    }
}

// Function to simulate the RSynaptic neuron model
void rsynaptic_forward(float input, float *syn, float *mem, float *spk, const float alpha, const float beta, const float threshold, const float *wrec) {
    // Update synaptic current
    *syn = alpha * (*syn) + input + (*wrec) * (*spk);

    // Update membrane potential
    *mem = beta * (*mem) + (*syn);

    // Generate spike if membrane potential exceeds threshold
    if (*spk == 1.0) {
        *mem -= threshold; // Reset by subtraction
    }
    
    if (*mem > threshold) {
        *spk = 1.0;
    } else {
        *spk = 0.0;
    }
}

// Function to simulate the RSynaptic_storklike neuron model
void rsynaptic_storklike_forward(float input, float *syn, float *mem, float *spk, const float alpha, const float beta, const float threshold, const float *wrec) {
    // Update synaptic current using previous syn
    float prev_syn = *syn;
    *syn = alpha * (*syn) + input + (*wrec) * (*spk);

    // Update membrane potential using previous syn
    float prev_mem = *mem;
    *mem = beta * (*mem) + (1-beta) * prev_syn; // scl_mem = 1-dcy_mem = 1-beta

    // Generate spike if membrane potential exceeds threshold
    if (prev_mem > threshold) {
        *spk = 1.0;
    } else {
        *spk = 0.0;
    }

    if (*spk == 1.0) {
        *mem -= threshold; // Reset by subtraction
    }
}

// Main application function
void app_main(void)
{
    ESP_LOGI(TAG, "Initializing RSNN...");

    uint16_t input_z[T][INPUT_NEURONS_NUM] = {{0},{3},{3},{3},{3},{0},{0},{0},{0},{0},{0}};

    const float alpha = 0.6703; //dcy_syn
    const float beta = 0.8187; //dcy_mem
    const float threshold = 1.0;
    float syn = 0.0;
    float mem = 0.0;
    float spk = 0.0;
    const float wrec = 1.0; // Example recurrent weight

    for(uint16_t i = 0; i < T; ++i) // for each timestep
    {
        ESP_LOGI(TAG, "  t: %d", i);
        ESP_LOGI(TAG, "  x: %d", input_z[i][0]);
        
        // Update Synaptic
        // synaptic_forward(input_z[i][0], &syn, &mem, alpha, beta, threshold, &spk);

        // Update RSynaptic
        // rsynaptic_forward(input_z[i][0], &syn, &mem, &spk, alpha, beta, threshold, &wrec);

        // Update RSynaptic_storklike
        rsynaptic_storklike_forward(input_z[i][0], &syn, &mem, &spk, alpha, beta, threshold, &wrec);

        // Print output
        ESP_LOGI(TAG, "syn: %f", syn);
        ESP_LOGI(TAG, "mem: %f", mem);
        ESP_LOGI(TAG, "spk: %f", spk);
        ESP_LOGI(TAG, "----------------------");
    }
}
