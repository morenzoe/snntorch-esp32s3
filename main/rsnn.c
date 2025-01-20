#include "rsnn.h"
#include <stdlib.h> // For random numbers
#include "freertos/FreeRTOS.h" // For delay function
#include "freertos/task.h" // For delay function
#include "esp_log.h" // For logging

static const char *TAG = "DEPLOY-ZENKE";

// Function to simulate the synaptic neuron model
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

// Function to reset synaptic and membrane potentials
void reset_mem(float *syn, float *mem) {
    *syn = 0.0;
    *mem = 0.0;
}

// Main application function
void app_main(void)
{
    ESP_LOGI(TAG, "Initializing RSNN...");

    uint16_t input_z[T][INPUT_NEURONS_NUM] = {{0},{3},{3},{3},{3},{0},{0},{0},{0},{0},{0}};

    float alpha = 0.9;
    float beta = 0.8;
    float threshold = 1.0;
    float syn = 0.0;
    float mem = 0.0;
    float spk = 0.0;

    for(uint16_t i = 0; i < T; ++i) // for each timestep
    {
        ESP_LOGI(TAG, "  t: %d", i);
        ESP_LOGI(TAG, "  x: %d", input_z[i][0]);
        
        // Update RSNN
        synaptic_forward(input_z[i][0], &syn, &mem, alpha, beta, threshold, &spk);

        // Print output
        ESP_LOGI(TAG, "syn: %f", syn);
        ESP_LOGI(TAG, "mem: %f", mem);
        ESP_LOGI(TAG, "spk: %f", spk);
        ESP_LOGI(TAG, "----------------------");
    }
}
