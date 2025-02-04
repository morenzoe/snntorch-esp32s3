#include "input.h"
#include "state_dict.h"
// #include "state_dict_example.h"
#include "rsnn.h"
#include <stdlib.h> // For random numbers
#include "freertos/FreeRTOS.h" // For delay function
#include "freertos/task.h" // For delay function
#include "esp_log.h" // For logging
#include <string.h> // For memcpy function

static const char *TAG = "";

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
void rsynaptic_storklike_forward(float *input, float *syn, float *mem, unsigned char *spk, const float *alpha, const float *beta, const float threshold, const float *wrec, int size) {
    unsigned char prev_spk[size / 8];
    memcpy(prev_spk, spk, (size / 8) * sizeof(spk[0])); // Save spk array as prev_spk

    for (int i = 0; i < size; i++) { // loop through neuron input
        // Update synaptic current using previous syn and all-to-all connections
        float prev_syn = syn[i];
        syn[i] = alpha[i] * syn[i] + input[i];
        for (int j = 0; j < size; j++) { // loop through neuron recurrent input
            if (prev_spk[j / 8] & (1 << (j % 8))) {
                syn[i] += wrec[i * size + j];
            }
        }

        // Update membrane potential using previous syn
        float prev_mem = mem[i];
        mem[i] = beta[i] * mem[i] + (1 - beta[i]) * prev_syn; // scl_mem = 1-dcy_mem = 1-beta

        // Generate spike if membrane potential exceeds threshold
        if (prev_mem > threshold) {
            spk[i / 8] |= (1 << (i % 8));
        } else {
            spk[i / 8] &= ~(1 << (i % 8));
        }

        if (spk[i / 8] & (1 << (i % 8))) {
            // mem[i] -= threshold; // Reset by subtraction
            mem[i] = 0; // Reset to zero
        }
    }
}

// Function to perform a linear transformation
void linear(const unsigned char *input, const float *weight, float *output, int input_size, int hidden_size) {
    for (int i = 0; i < hidden_size; ++i) { //hidden_size = REC_NEURONS_NUM = 8
        output[i] = 0.0; //output = float cur1[REC_NEURONS_NUM] = {0.0};
        for (int j = 0; j < input_size / 8; ++j) { //input_size = INPUT_NEURONS_NUM = 8
            for (int k = 0; k < 8; ++k) {
                if (input[j] & (1 << k)) { //input = unsigned char input_old[INPUT_NEURONS_NUM / 8] = {0b00000000};
                    // ESP_LOGI(TAG, "Access i: %d, j: %d, k: %d", i, j, k);
                    // ESP_LOGI(TAG, "input[j]: %d", input[j]);
                    output[i] += weight[i * input_size + j * 8 + k]; // weight = const float fc_hidden_weight[REC_NEURONS_NUM][INPUT_NEURONS_NUM]
                }
            }
        }
    }
}

// Main application function
void app_main(void)
{
    ESP_LOGI(TAG, "Initializing RSNN...");
    ESP_LOGI(TAG, "INPUT_NEURONS_NUM: %d", INPUT_NEURONS_NUM);
    ESP_LOGI(TAG, "REC_NEURONS_NUM: %d", REC_NEURONS_NUM);
    // ESP_LOGI(TAG, "Size of %d", sizeof(input_z));

    // ESP_LOGI(TAG, "input_old: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", input_old[0], input_old[1], input_old[2], input_old[3], input_old[4], input_old[5], input_old[6], input_old[7], input_old[8], input_old[9], input_old[10], input_old[11], input_old[12]);
    // ESP_LOGI(TAG, "b cur1: %f, %f, %f, %f, %f, %f, %f, %f", cur1[0], cur1[1], cur1[2], cur1[3], cur1[4], cur1[5], cur1[6], cur1[7]);

    // linear(input_old, (const float *)fc_hidden_weight, cur1, INPUT_NEURONS_NUM, REC_NEURONS_NUM);

    // ESP_LOGI(TAG, "a cur1: %f, %f, %f, %f, %f, %f, %f, %f", cur1[0], cur1[1], cur1[2], cur1[3], cur1[4], cur1[5], cur1[6], cur1[7]);

    for(int i = 0; i < T; ++i) // for each timestep
    {
        // start time
        ESP_LOGI(TAG, "     t: %d", i);
        // ESP_LOGI(TAG, "input_z: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", input_z[i][0][0], input_z[i][0][1], input_z[i][0][2], input_z[i][0][3], input_z[i][0][4], input_z[i][0][5], input_z[i][0][6], input_z[i][0][7], input_z[i][0][8], input_z[i][0][9], input_z[i][0][10], input_z[i][0][11]);
        // ESP_LOGI(TAG, "input_z: %d", input_z[i][0][0]);

        // Unpack input bits to float array
        // unpack_input(input_z[i / 8][0], unpacked_input, INPUT_NEURONS_NUM, i%8);
        // ESP_LOGI(TAG, "unpack: %f, %f, %f", unpacked_input[0], unpacked_input[1], unpacked_input[2]);

        // Fully connected layer1
        // ESP_LOGI(TAG, "Linear1");
        linear(input_old, (const float *)fc_hidden_weight, cur1, INPUT_NEURONS_NUM, REC_NEURONS_NUM);
        // memcpy(input, unpacked_input, INPUT_NEURONS_NUM * sizeof(unpacked_input[0]));
        memcpy(input_old, input_z[i][0], INPUT_NEURONS_NUM / 8 * sizeof(input_z[i][0][0]));
        
        // Print output
        // ESP_LOGI(TAG, "  cur1: %f, %f, %f, %f, %f, %f, %f, %f", cur1[0], cur1[1], cur1[2], cur1[3], cur1[4], cur1[5], cur1[6], cur1[7]);

        // Update RSynaptic_storklike
        // memcpy(syn1_old, syn1, REC_NEURONS_NUM * sizeof(syn1[0]));
        // memcpy(mem1_old, mem1, REC_NEURONS_NUM * sizeof(mem1[0]));
        // ESP_LOGI(TAG, "RSynaptic");
        memcpy(spk1_old, spk1, REC_NEURONS_NUM / 8 * sizeof(spk1[0]));
        rsynaptic_storklike_forward(cur1, syn1, mem1, spk1, alpha1, beta1, threshold1, (const float *)lif_hidden_recurrent_weight, REC_NEURONS_NUM);

        // Print output
        // ESP_LOGI(TAG, "  syn1: %f, %f, %f, %f, %f, %f, %f, %f", syn1[0], syn1[1], syn1[2], syn1[3], syn1[4], syn1[5], syn1[6], syn1[7]);
        // ESP_LOGI(TAG, "  mem1: %f, %f, %f, %f, %f, %f, %f, %f", mem1[0], mem1[1], mem1[2], mem1[3], mem1[4], mem1[5], mem1[6], mem1[7]);
        // // ESP_LOGI(TAG, "  spk1: %d, %d, %d, %d, %d, %d, %d, %d", spk1[0], spk1[1], spk1[2], spk1[3], spk1[4], spk1[5], spk1[6], spk1[7]);
        // ESP_LOGI(TAG, "  spk1: %d", spk1[0]);

        // Reset output averaging variable
        // ESP_LOGI(TAG, "Readout Head");
        output[0] = 0.0;
        output[1] = 0.0;
        // Loop through 5 readout heads
        for (int j = 0; j < READOUT_HEAD_NUM; ++j) {
            // Fully connected layer2
            // ESP_LOGI(TAG, "Linear2");
            linear(spk1_old, (const float *)fc_outputs_weight[j], cur2[j], REC_NEURONS_NUM, OUTPUT_NEURONS_NUM);

            // Print output
            // ESP_LOGI(TAG, "cur2_%d: %f, %f", j, cur2[j][0], cur2[j][1]);

            // Update Synaptic_storklike
            // ESP_LOGI(TAG, "Synaptic");
            synaptic_storklike_forward(cur2[j], syn2[j], mem2[j], alpha2[j], beta2[j], threshold2, OUTPUT_NEURONS_NUM);

            // Print output
            // ESP_LOGI(TAG, "syn2_%d: %f, %f", j, syn2[j][0], syn2[j][1]);
            // ESP_LOGI(TAG, "mem2_%d: %f, %f", j, mem2[j][0], mem2[j][1]);

            output[0] += mem2[j][0];
            output[1] += mem2[j][1];

        }

        output[0] /= READOUT_HEAD_NUM;
        output[1] /= READOUT_HEAD_NUM;
        // stop time

        // Print output
        ESP_LOGI(TAG, "output: %f, %f", output[0], output[1]); // print time

        // ESP_LOGI(TAG, "------------------------------------");
    }
}
