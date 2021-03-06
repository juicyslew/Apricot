
#ifndef CALSA_ENVGEN
#define CALSA_ENVGEN 1

#include "global_definitions.h"
#include "note.h"
#include <cstdlib>
#include <limits>


struct envinfo_st{ //This 
  //float start_val; //0 to 1

  //OPERATOR//
  float attack_time; //time in seconds //TODO: make this 0 to 1 and then map to an exponential function of 0 seconds to some amount of seconds
  double attack_val; //some value from 0 to 1; (for volume map this to -60 to 0 dB and then do 10^(x/10) to remap to intensity)

  float decay_time; //decay to sus_vol

  double sustain_val; //on operat

  double release_val; //on operator
  float release_time; //on operator
  
  envinfo_st(){

  }

};

struct envgen_st{
  //NOTE//
  long long timer; //on note
  double interval; //on note
  double val; //saves the current val of the envelope // on note

  enum envstate state; //on note
  envgen_st(){

  }
};

envgen_t init_envelope();

envinfo_t init_envelope_info();

void set_release_expo(envgen_t env, envinfo_t info);
void set_release_linear(envgen_t env, envinfo_t info);

float use_envelope_expo(envgen_t env, envinfo_t info);
float use_envelope_linear(envgen_t env, envinfo_t info);

double compute_interval_expo(double nextlevel, envgen_t env);
double compute_interval_linear(double nextlevel, envgen_t env);

#endif