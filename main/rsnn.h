#ifndef RSNN_H
#define RSNN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// RSNN CONFIG
#define INPUT_NEURONS_NUM 3
#define T 11 // total timesteps

/*****/
// VARIABLES AND BUFFERS


/*****/
// FUNCTIONS
void reset_mem(float *syn, float *mem);
void synaptic_forward(unsigned short int input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold);
void rsynaptic_forward(unsigned short int input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold, const float *wrec);
void synaptic_storklike_forward(unsigned short int *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold, int size);
void rsynaptic_storklike_forward(unsigned short int *input, float *syn, float *mem, float *spk, const float *alpha, const float *beta, const float *threshold, const float wrec[INPUT_NEURONS_NUM][INPUT_NEURONS_NUM], int size);

#endif // RSNN_H