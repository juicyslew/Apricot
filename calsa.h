

#ifndef CALSA_MAIN
#define CALSA_MAIN 1

#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <string>
#include <pthread.h>
#include <time.h>
#include <deque>
#include <list>
#include <unordered_map>
#include "global_definitions.h"
#include "note.h"
#include "operator.h"


struct ev_key {
  int code;
  int type;
};

struct calsa_st {
  std::deque<struct ev_key> key_events;
  std::unordered_map<int, note_t> active_notes;
  std::list<note_t> passive_notes;
  
  operator_t* optors;

  calsa_st(){
  }
  //std::list<note_t> sounds;
  //note_t* sounds;
};

//Initialize controller
calsa_t calsa_init(void);

//Function to return next sound sample
unsigned short* get_samples(calsa_t calsa, long long t);


#endif