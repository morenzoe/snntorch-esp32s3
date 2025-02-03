#ifndef RSNN_H
#define RSNN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// RSNN CONFIG
// #define INPUT_NEURONS_NUM 3
// #define REC_NEURONS_NUM 2
// #define OUTPUT_NEURONS_NUM 1
// #define T 11 // total timesteps

/*****/
// VARIABLES AND BUFFERS
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
//     {{1,1,0}},
//     {{1,0,0}},
//     {{1,0,0}},
//     {{1,0,0}},
//     {{0,1,1}},
//     {{0,0,1}},
//     {{0,0,1}},
//     {{0,0,1}},
//     {{0,1,0}},
//     {{0,0,0}}};

/*****/
// FUNCTIONS
void reset_mem(float *syn, float *mem);
// void synaptic_storklike_forward(float *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float threshold, int size);
void synaptic_storklike_forward(float *input, float *syn, float *mem, const float *alpha, const float *beta, const float threshold, int size);
void rsynaptic_storklike_forward(float *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float threshold, const float *wrec, int size);
void linear(float *input, const float *weight, float *output, int input_size, int hidden_size);
void unpack_input(const unsigned char *packed_input, float *unpacked_input, int size, int pos);

#endif // RSNN_H