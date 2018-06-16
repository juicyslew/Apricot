

#ifndef ALSA_NOTE
#define ALSA_NOTE 1

#include <math.h>
#include <iostream>
#include "global_definitions.h"
#include "calsa.h"
#include "Env.h"


struct note_st{
  //int held; // 1 = held, 2 = just let go, -1 = released
  float freq;
  envgen_t* volenv;
  envgen_t* freqenv;
  float* phaseincs;
  //float* volrels; //Don't know how I feel about putting this here, but works for now
  float* op_outs; //Don't know how I feel about putting this here, but works for now
  //TODO: move volrels and op_outs back into operator somehow, without loosing too much efficiency, or recursive FM behavior
  //float* freqrel;
  //float* phaserel;
  //float vol_attack; -> goes in env
  //float vol_release; -> goes in env
  //enum waveform style;

  long long press_t; //Note is pressed;
  long long release_t; //Note is released;
  long long end_t; //Note is deleted;
  //float s_rate;

  note_st(){
    
  }
};


note_t note_init(calsa_t calsa, float freq, long long t);

//Function to generate notes for current sample read
void calsa_read_notes(calsa_t calsa, long long t);

void calsa_note_trimming(calsa_t calsa, long long t);

#endif