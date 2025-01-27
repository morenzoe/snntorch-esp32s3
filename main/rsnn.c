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

// Function to simulate the Synaptic_storklike neuron model with array input and output
// void synaptic_storklike_forward(float *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float threshold, int size) {
void synaptic_storklike_forward(float *input, float *syn, float *mem, const float *alpha, const float *beta, const float threshold, int size) {
    for (int i = 0; i < size; i++) {
        // Update synaptic current
        float prev_syn = syn[i];
        syn[i] = alpha[i] * syn[i] + input[i];

        // Update membrane potential
        // float prev_mem = mem[i];
        mem[i] = beta[i] * mem[i] + (1 - beta[i]) * prev_syn; // scl_mem = 1-dcy_mem = 1-beta

        // // Generate spike if membrane potential exceeds threshold
        // if (prev_mem > threshold) {
        //     spk[i] = 1.0;
        // } else {
        //     spk[i] = 0.0;
        // }

        // if (spk[i] == 1.0) {
        //     mem[i] -= threshold; // Reset by subtraction
        // }
    }
}

// Function to simulate the RSynaptic_storklike neuron model with array input and output
void rsynaptic_storklike_forward(float *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float threshold, const float *wrec, int size) {
    float prev_spk[size];
    memcpy(prev_spk, spk, size * sizeof(spk[0])); // Save spk array as prev_spk

    for (int i = 0; i < size; i++) { // loop through neuron input
        // Update synaptic current using previous syn and all-to-all connections
        float prev_syn = syn[i];
        syn[i] = alpha[i] * syn[i] + input[i];
        for (int j = 0; j < size; j++) { // loop through neuron recurrent input
            syn[i] += wrec[i * size + j] * prev_spk[j];
        }

        // Update membrane potential using previous syn
        float prev_mem = mem[i];
        mem[i] = beta[i] * mem[i] + (1 - beta[i]) * prev_syn; // scl_mem = 1-dcy_mem = 1-beta

        // Generate spike if membrane potential exceeds threshold
        if (prev_mem > threshold) {
            spk[i] = 1.0;
        } else {
            spk[i] = 0.0;
        }

        if (spk[i] == 1.0) {
            mem[i] -= threshold; // Reset by subtraction
        }
    }
}

// Function to perform a linear transformation
void linear(float *input, const float *weight, float *output, int input_size, int hidden_size) {
    for (int i = 0; i < hidden_size; ++i) {
        output[i] = 0.0;
        for (int j = 0; j < input_size; ++j) {
            output[i] += input[j] * weight[i * input_size + j];
        }
    }
}

// Main application function
void app_main(void)
{
    ESP_LOGI(TAG, "Initializing RSNN...");
    
    // float input[2] = {1.0, 1.0}; // Example input
    // float output[3] = {0.0, 0.0, 0.0}; // Output array

    // // Call the linear function
    // linear(input, (const float *)weight, output, 2, 3);

    // // Print the output
    // ESP_LOGI(TAG, "Linear output: %f, %f, %f", output[0], output[1], output[2]);

    // ...existing code...

    for(uint16_t i = 0; i < T; ++i) // for each timestep
    {
        ESP_LOGI(TAG, "     t: %d", i);

        ESP_LOGI(TAG, "     x: %f, %f, %f", input_z[i][0][0], input_z[i][0][1], input_z[i][0][2]);
        
        // Fully connected layer1
        linear(input_z[i][0], (const float *)fc_hidden, cur1, INPUT_NEURONS_NUM, REC_NEURONS_NUM);
        
        // Print output
        ESP_LOGI(TAG, "  cur1: %f, %f", cur1[0], cur1[1]);

        // Update RSynaptic_storklike
        rsynaptic_storklike_forward(cur1, syn1, mem1, spk1, alpha1, beta1, threshold1, (const float *)wrec, REC_NEURONS_NUM);

        // Print output
        ESP_LOGI(TAG, "  syn1: %f, %f", syn1[0], syn1[1]);
        ESP_LOGI(TAG, "  mem1: %f, %f", mem1[0], mem1[1]);
        ESP_LOGI(TAG, "  spk1: %f, %f", spk1[0], spk1[1]);

        // Loop through 5 readout heads
        for (int j = 0; j < READOUT_HEAD_NUM; ++j) {
            // Fully connected layer2
            linear(spk1, (const float *)fc_outputs[j], cur2[j], REC_NEURONS_NUM, OUTPUT_NEURONS_NUM);

            // Print output
            ESP_LOGI(TAG, "cur2_%d: %f", j, cur2[j][0]);

            // Update Synaptic_storklike
            // synaptic_storklike_forward(cur2[j], syn2[j], mem2[j], spk2[j], alpha2[j], beta2[j], threshold2, OUTPUT_NEURONS_NUM);
            synaptic_storklike_forward(cur2[j], syn2[j], mem2[j], alpha2[j], beta2[j], threshold2, OUTPUT_NEURONS_NUM);

            // Print output
            ESP_LOGI(TAG, "syn2_%d: %f", j, syn2[j][0]);
            ESP_LOGI(TAG, "mem2_%d: %f", j, mem2[j][0]);
            // ESP_LOGI(TAG, "spk2_%d: %f", j, spk2[j][0]);

            output[0] += mem2[j][0];
            output[1] += mem2[j][1];

        }

        output[0] /= READOUT_HEAD_NUM;
        output[1] /= READOUT_HEAD_NUM;

        // Print output
        ESP_LOGI(TAG, "output: %f, %f", output[0], output[1]);

        ESP_LOGI(TAG, "----------------------");
    }
}
