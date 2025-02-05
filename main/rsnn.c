#include "input.h"
#include "state_dict.h"
#include "rsnn.h"

// Function to simulate the Synaptic_storklike neuron model with array input and output
void synaptic_storklike_forward(float *input, float *syn, float *mem, const float *alpha, const float *beta, const float threshold, int size) {
    for (int i = 0; i < size; i++) {
        // Update synaptic current
        float prev_syn = syn[i];
        syn[i] = alpha[i] * syn[i] + input[i];

        // Update membrane potential
        mem[i] = beta[i] * mem[i] + (1 - beta[i]) * prev_syn; // scl_mem = 1-dcy_mem = 1-beta
    }
}

// Function to simulate the RSynaptic_storklike neuron model with array input and output
void rsynaptic_storklike_forward(float *input, float *syn, float *mem, uint8_t *spk, const float *alpha, const float *beta, const float threshold, const float *wrec, int size) {
    uint8_t prev_spk[size / 8];
    memcpy(prev_spk, spk, (size / 8) * sizeof(spk[0])); // Save spk array as prev_spk

    int spk_count = 0;
    int spk_indices[size];  // Max size but will only use a few

    for (int j = 0; j < size; j++) {
        if (prev_spk[j / 8] & (1 << (j % 8))) {
            spk_indices[spk_count++] = j;
        }
    }

    for (int i = 0; i < size; i++) { // loop through neuron input
        // Update synaptic current using previous syn and all-to-all connections
        float prev_syn = syn[i];
        syn[i] = alpha[i] * syn[i] + input[i];
        for (int k = 0; k < spk_count; k++) {  // Loop only through spiking neurons
            int j = spk_indices[k];
            syn[i] += wrec[i * size + j];  // Update synaptic current from spiking neurons
        }
        // for (int j = 0; j < size; j++) { // loop through neuron recurrent input
        //     if (prev_spk[j / 8] & (1 << (j % 8))) {
        //         syn[i] += wrec[i * size + j];
        //     }
        // }

        // Update membrane potential using previous syn
        float prev_mem = mem[i];
        mem[i] = beta[i] * mem[i] + (1 - beta[i]) * prev_syn;

        // Generate spike if membrane potential exceeds threshold
        if (prev_mem <= threshold) {
            spk[i / 8] &= ~(1 << (i % 8));
        } else {
            spk[i / 8] |= (1 << (i % 8));
        }

        if (spk[i / 8] & (1 << (i % 8))) {
            mem[i] = 0; // Reset to zero
        }
    }
}

// Function to perform a linear transformation
void linear(const uint8_t *input, const float *weight, float *output, int input_size, int hidden_size) {
    int in_count = 0;
    int in_indices[input_size];  // Max size but will only use a few
    
    for (int j = 0; j < input_size / 8; ++j) {
            for (int k = 0; k < 8; ++k) {
                if (input[j] & (1 << k)) {
                    in_indices[in_count++] = j * 8 + k;
                    // output[i] += weight[i * input_size + j * 8 + k]; 
                }
            }
        }
    
    for (int i = 0; i < hidden_size; ++i) {
        output[i] = 0.0;
        for (int k = 0; k < in_count; k++) {  // Loop only through spiking neurons
            int in_idx = in_indices[k];
            output[i] += weight[i * input_size + in_idx];  // Update output from spiking input
        }
        // for (int j = 0; j < input_size / 8; ++j) {
        //     for (int k = 0; k < 8; ++k) {
        //         if (input[j] & (1 << k)) {
        //             output[i] += weight[i * input_size + j * 8 + k]; 
        //         }
        //     }
        // }
    }
}

static const char *TAG = "indy_20160622_01";

// Main application function
void app_main(void)
{
    run_time = esp_timer_get_time();
    ESP_LOGI(TAG, "optimize linear1");
    printf("i, x-axis, y-axis, inference_time, linear1_time, memcpy1_time, memcpy2_time, rsynaptic_time, readout_time\n");
    

    for(int i = 0; i < 100; ++i) // for each timestep
    {
        // start time
        inference_time = esp_timer_get_time();
        
        linear1_time = esp_timer_get_time();
        linear(input_old, (const float *)fc_hidden_weight, cur1, INPUT_NEURONS_NUM, REC_NEURONS_NUM);
        linear1_time = esp_timer_get_time() - linear1_time;
        
        memcpy1_time = esp_timer_get_time();
        memcpy(input_old, input_z[i][0], INPUT_NEURONS_NUM / 8 * sizeof(input_z[i][0][0]));
        memcpy1_time = esp_timer_get_time() - memcpy1_time;
        
        memcpy2_time = esp_timer_get_time();
        memcpy(spk1_old, spk1, REC_NEURONS_NUM / 8 * sizeof(spk1[0]));
        memcpy2_time = esp_timer_get_time() - memcpy2_time;

        rsynaptic_time = esp_timer_get_time();
        rsynaptic_storklike_forward(cur1, syn1, mem1, spk1, alpha1, beta1, threshold1, (const float *)lif_hidden_recurrent_weight, REC_NEURONS_NUM);
        rsynaptic_time = esp_timer_get_time() - rsynaptic_time;

        readout_time = esp_timer_get_time();
        output[0] = 0.0;
        output[1] = 0.0;
        // Loop through 5 readout heads
        for (int j = 0; j < READOUT_HEAD_NUM; ++j) {
            linear(spk1_old, (const float *)fc_outputs_weight[j], cur2[j], REC_NEURONS_NUM, OUTPUT_NEURONS_NUM);

            synaptic_storklike_forward(cur2[j], syn2[j], mem2[j], alpha2[j], beta2[j], threshold2, OUTPUT_NEURONS_NUM);

            output[0] += mem2[j][0];
            output[1] += mem2[j][1];

        }
        output[0] /= READOUT_HEAD_NUM;
        output[1] /= READOUT_HEAD_NUM;
        readout_time = esp_timer_get_time() - readout_time;

        // stop time
        inference_time = esp_timer_get_time() - inference_time;

        // Print output
        printf("%d, %f, %f, %lld, %lld, %lld, %lld, %lld, %lld\n", i, output[0], output[1], inference_time, linear1_time, memcpy1_time, memcpy2_time, rsynaptic_time, readout_time); // print time
    }

    run_time = esp_timer_get_time() - run_time;
    ESP_LOGI(TAG, "Total run time: %lld us", run_time);
}
