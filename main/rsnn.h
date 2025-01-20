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

void synaptic_forward(float input, float *syn, float *mem, float alpha, float beta, float threshold, float *spk);
void reset_mem(float *syn, float *mem);

#endif // RSNN_H