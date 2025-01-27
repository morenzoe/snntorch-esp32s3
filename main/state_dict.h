#define REC_NEURONS_NUM 2
#define OUTPUT_NEURONS_NUM 1
#define READOUT_HEAD_NUM 5

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

// Readout Head
// Fully Connected 2
float cur2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM] = {{0.0}};
const float fc_outputs[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM][REC_NEURONS_NUM] = {
    {{1.0, 1.0}},
    {{1.0, 1.0}},
    {{1.0, 1.0}},
    {{1.0, 1.0}},
    {{1.0, 1.0}}
};

// Synaptic
const float alpha2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM] = {{0.6703}}; //dcy_syn
const float beta2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM] = {{0.8187}}; //dcy_mem
const float threshold2 = 1.0;
float syn2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM] = {{0.0}};
float mem2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM] = {{0.0}};
// float spk2[READOUT_HEAD_NUM][OUTPUT_NEURONS_NUM] = {{0.0}};

// Output
float output[OUTPUT_NEURONS_NUM] = {0.0};