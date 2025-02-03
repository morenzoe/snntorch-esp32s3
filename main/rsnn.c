#include "input.h"
#include "state_dict.h"
#include "rsnn.h"
#include <stdlib.h> // For random numbers
#include "freertos/FreeRTOS.h" // For delay function
#include "freertos/task.h" // For delay function
#include "esp_log.h" // For logging
#include <string.h> // For memcpy function

static const char *TAG = "";

// Function to reset synaptic and membrane potentials
void reset_mem(float *syn, float *mem) {
    *syn = 0.0;
    *mem = 0.0;
}

// Function to unpack input bits into float array
void unpack_input(const unsigned char *packed_input, float *unpacked_input, int size, int pos) {
    for (int i = 0; i < size; ++i) {
        unpacked_input[i] = (packed_input[i] & (1 << (7-pos))) ? 1.0f : 0.0f;
    }
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
            // mem[i] -= threshold; // Reset by subtraction
            mem[i] = 0; // Reset to zero
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

    for(int i = 0; i < num_steps; ++i) // for each timestep
    {
        ESP_LOGI(TAG, "     t: %d", i);

        // Unpack input bits to float array
        unpack_input(input_z[i / 8][0], unpacked_input, INPUT_NEURONS_NUM, i%8);
        ESP_LOGI(TAG, "unpack: %f, %f, %f", unpacked_input[0], unpacked_input[1], unpacked_input[2]);

        // Fully connected layer1
        linear(input, (const float *)fc_hidden_weight, cur1, INPUT_NEURONS_NUM, REC_NEURONS_NUM);
        memcpy(input, unpacked_input, INPUT_NEURONS_NUM * sizeof(unpacked_input[0]));
        
        // Print output
        ESP_LOGI(TAG, "  cur1: %f, %f, %f, %f, %f", cur1[0], cur1[1], cur1[2], cur1[3], cur1[4]);

        // Update RSynaptic_storklike
        // memcpy(syn1_old, syn1, REC_NEURONS_NUM * sizeof(syn1[0]));
        // memcpy(mem1_old, mem1, REC_NEURONS_NUM * sizeof(mem1[0]));
        memcpy(spk1_old, spk1, REC_NEURONS_NUM * sizeof(spk1[0]));
        rsynaptic_storklike_forward(cur1, syn1, mem1, spk1, alpha1, beta1, threshold1, (const float *)lif_hidden_recurrent_weight, REC_NEURONS_NUM);

        // Print output
        ESP_LOGI(TAG, "  syn1: %f, %f, %f, %f, %f", syn1[0], syn1[1], syn1[2], syn1[3], syn1[4]);
        ESP_LOGI(TAG, "  mem1: %f, %f, %f, %f, %f", mem1[0], mem1[1], mem1[2], mem1[3], mem1[4]);
        ESP_LOGI(TAG, "  spk1: %f, %f, %f, %f, %f", spk1[0], spk1[1], spk1[2], spk1[3], spk1[4]);

        // Reset output averaging variable
        float output[OUTPUT_NEURONS_NUM] = {0.0};
        // Loop through 5 readout heads
        for (int j = 0; j < READOUT_HEAD_NUM; ++j) {
            // Fully connected layer2
            linear(spk1_old, (const float *)fc_outputs_weight[j], cur2[j], REC_NEURONS_NUM, OUTPUT_NEURONS_NUM);

            // Print output
            ESP_LOGI(TAG, "cur2_%d: %f, %f", j, cur2[j][0], cur2[j][1]);

            // Update Synaptic_storklike
            synaptic_storklike_forward(cur2[j], syn2[j], mem2[j], alpha2[j], beta2[j], threshold2, OUTPUT_NEURONS_NUM);

            // Print output
            ESP_LOGI(TAG, "syn2_%d: %f, %f", j, syn2[j][0], syn2[j][1]);
            ESP_LOGI(TAG, "mem2_%d: %f, %f", j, mem2[j][0], mem2[j][1]);

            output[0] += mem2[j][0];
            output[1] += mem2[j][1];

        }

        output[0] /= READOUT_HEAD_NUM;
        output[1] /= READOUT_HEAD_NUM;

        // Print output
        ESP_LOGI(TAG, "output: %f, %f", output[0], output[1]);

        ESP_LOGI(TAG, "------------------------------------");
    }
}
