#include "input.h"
#define REC_NEURONS_NUM 1024
#define OUTPUT_NEURONS_NUM 2
#define READOUT_HEAD_NUM 5

// Input
extern unsigned char input_old[INPUT_NEURONS_NUM / 8];

// Fully Connected 1
extern float cur1[REC_NEURONS_NUM];
extern const float fc_hidden_weight[REC_NEURONS_NUM][INPUT_NEURONS_NUM];

// RSynaptic
extern const float alpha1[REC_NEURONS_NUM]; //dcy_syn
extern const float beta1[REC_NEURONS_NUM]; //dcy_mem
extern const float threshold1;
extern float syn1[REC_NEURONS_NUM];
extern float mem1[REC_NEURONS_NUM];
extern unsigned char spk1[REC_NEURONS_NUM / 8];
extern unsigned char spk1_old[REC_NEURONS_NUM / 8];
extern const float lif_hidden_recurrent_weight[REC_NEURONS_NUM][REC_NEURONS_NUM];

// Readout Head
// Fully Connected 2
extern float cur2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM];
extern const float fc_outputs_weight[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM][REC_NEURONS_NUM];

// Synaptic
extern const float alpha2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM]; //dcy_syn
extern const float beta2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM]; //dcy_mem
extern const float threshold2;
extern float syn2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM];
extern float mem2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM];

// Output
extern float output[OUTPUT_NEURONS_NUM];