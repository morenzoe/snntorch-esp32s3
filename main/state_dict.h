#define REC_NEURONS_NUM 2
#define OUTPUT_NEURONS_NUM 1

// Fully Connected 1
float cur1[REC_NEURONS_NUM] = {0.0};
const float fc_hidden[REC_NEURONS_NUM][INPUT_NEURONS_NUM] = {
    {1.0, 1.0, 0.0},
    {0.0, 1.0, 1.0}
};

// RSynaptic
const float alpha1[REC_NEURONS_NUM] = {0.6703, 0.6703}; //dcy_syn
const float beta1[REC_NEURONS_NUM] = {0.8187, 0.8187}; //dcy_mem
const float threshold1 = 1.0;
float syn1[REC_NEURONS_NUM] = {0.0};
float mem1[REC_NEURONS_NUM] = {0.0};
float spk1[REC_NEURONS_NUM] = {0.0};
const float wrec[REC_NEURONS_NUM][REC_NEURONS_NUM] = {
    {1.0, 1.0},
    {1.0, 1.0}
};

// Readout Head 0
// Fully Connected 2
float cur2_0[OUTPUT_NEURONS_NUM] = {0.0};
const float fc_outputs_0[OUTPUT_NEURONS_NUM][REC_NEURONS_NUM] = {
    {1.0, 1.0}
};

// Synaptic
const float alpha2_0[OUTPUT_NEURONS_NUM] = {0.6703}; //dcy_syn
const float beta2_0[OUTPUT_NEURONS_NUM] = {0.8187}; //dcy_mem
const float threshold2_0 = 1.0;
float syn2_0[OUTPUT_NEURONS_NUM] = {0.0};
float mem2_0[OUTPUT_NEURONS_NUM] = {0.0};
float spk2_0[OUTPUT_NEURONS_NUM] = {0.0};