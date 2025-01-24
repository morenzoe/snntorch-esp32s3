#ifndef RSNN_H
#define RSNN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// RSNN CONFIG
#define INPUT_NEURONS_NUM 1
# define T 11 // total timesteps

/*****/
// VARIABLES AND BUFFERS


/*****/
// FUNCTIONS
void reset_mem(float *syn, float *mem);
void synaptic_forward(float input, float *syn, float *mem, float alpha, float beta, float threshold, float *spk);
void rsynaptic_forward(float input, float *syn, float *mem, float *spk, float alpha, const float beta, const float threshold, const float *wrec);
void rsynaptic_storklike_forward(float input, float *syn, float *mem, float *spk, const float alpha, const float beta, const float threshold, const float *wrec);

#endif // RSNN_H