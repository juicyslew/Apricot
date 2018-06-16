#include "operator.h"

const float sin_mult = (2.0*M_PI/s_rate);

operator_t init_operator(void)
{
  operator_t optor = new operator_st(); //(operator_t)calloc(1, sizeof(operator_st));
  optor->style = sine;
  for (int i = 0; i < OPNUM; i++){
    optor->modreceive[i] = 0.0;
  }

  optor->volstatic = .15; //TODO: MAKE THIS INITIALIZE TO 1 FOR THE FIRST AND 0 FOR THE REST. (it'll make it nicer to test things and hear things.)
  optor->volinfo = init_envelope_info();

  optor->freqstatic = .5;
  optor->freqinfo = init_envelope_info();

  /*optor->phasestatic = 1;
  optor->phaseinfo = init_envelope_info();*/

  return optor;
}

operator_t* init_all_operators(void)
{
  operator_t* op_array = (operator_t*)malloc(OPNUM * sizeof(operator_t));
  
  for (int i = 0; i < OPNUM; i++){
    operator_t tmp = init_operator();
    op_array[i] = tmp;
  }
  return op_array;
}

void note_specific_update(operator_t* optors, note_t note, long long t){
  for (int i = 0; i < OPNUM; i++){
    float volmult = use_envelope_expo(note->volenv[i], optors[i]->volinfo);
    if (volmult == 0){
      continue;
    }
    float temp = 0;

    if (optors[i]->style == noise){
      temp = (float)rand() / (float)RAND_MAX;
    }else{
      float fm_acc = 0;
      for (int k = 0; k < OPNUM; k++){
        fm_acc += optors[i]->modreceive[k] * note->op_outs[k];
      }

      //printf("%f\n", optors[0]->modreceive[1]);
      float tmpfreq = note->freq * optors[i]->freqstatic;// * freq_mult;

      note->phaseincs[i] += tmpfreq * use_envelope_linear(note->freqenv[i], optors[i]->freqinfo)/s_rate;  //Generalize this

      switch(optors[i]->style)
      {
        case sine:
        {
          temp = (sin(t * sin_mult * tmpfreq + fm_acc + note->phaseincs[i])+1.0)/2.0;
          break;
        }
        case saw:
        {
          temp = fmod(t*(tmpfreq/s_rate) + fm_acc, 1.0);
          break;
        }
        case square:
        {
          temp = fmod(floor(t*(tmpfreq/s_rate)*2.0 + fm_acc), 2.0);
          break;
        }
      }
    }
    note->op_outs[i] = temp * volmult;
  }
}

void operator_update(calsa_t calsa, long long t){

  //TODO: CHECK IF OPTOR IS NOT BEING USED AT ALL AND ENTIRELY SKIP IT.
  //TODO: ADD RING MODULATION (RM)

  for(std::pair<int, note_t> n : calsa->active_notes){
    note_specific_update(calsa->optors, n.second, t);
  }
  for(note_t note : calsa->passive_notes){
    note_specific_update(calsa->optors, note, t);
  }
}

float* operator_bundled_op_out(calsa_t calsa){
  float * bundout = (float *) calloc(OPNUM, sizeof(float));
  for (int i = 0; i < OPNUM; i++){
    float bund = 0;
    for(auto it = calsa->active_notes.begin(); it != calsa->active_notes.end(); ){
      bund += it->second->op_outs[i];
      it++;
    }
    for(auto it = calsa->passive_notes.begin(); it != calsa->passive_notes.end(); ){
      bund += (*it)->op_outs[i];
      it++;
    }
    bundout[i] = bund * calsa->optors[i]->volstatic;
  }
  return bundout;
}