
#ifndef CALSA_OPERATOR
#define CALSA_OPERATOR 1

#include <math.h>
#include <cmath>
#include <cstdlib>
#include "global_definitions.h"
#include "calsa.h"
#include "Env.h"
#include "note.h"


struct operator_st{
  //GEN INFO
  enum waveform style;

  //FREQUENCY ENV
  envinfo_t freqinfo;
  float freqstatic;
  double phase_inc;

  //PHASE ENV
  envinfo_t phaseinfo;

  //VOLUME ENV;
  envinfo_t volinfo;
  float volstatic;

  //MOD ENV
  envinfo_t modenv;

  //"FREQUENCY MODULATOR"
  float modreceive[OPNUM];
  



  //OUTPUT
  //float* outputs; //each is a list corresponding with a note

  operator_st(){

  };
};

operator_t init_operator(void);

operator_t* init_all_operators(void);

void operator_update(calsa_t calsa, long long t);

float* operator_bundled_op_out(calsa_t calsa);

void note_specific_update(operator_t* optors, note_t note, long long t);

#endif