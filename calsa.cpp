

#include "calsa.h"

calsa_t calsa_init(void){
  srand(time(NULL));   // should only be called once
  
  calsa_t calsa = new calsa_st();

  /*------------ INITIALIZATION ------------*/

  calsa->optors = init_all_operators();

  //calsa->p=0;

  return calsa;
}

unsigned short* get_samples(calsa_t calsa, long long t){
  unsigned short *s = (unsigned short *)calloc(2, sizeof(unsigned short));
  const float vol = 32726.0; // the /2.0 is important
  //printf("1")
  //pthread_mutex_lock(&(calsa_lock));

  calsa_read_notes(calsa, t);
  
  calsa_note_trimming(calsa, t);

  operator_update(calsa, t);

  float * bundled_sound = operator_bundled_op_out(calsa);
  float temp = 0;
  for (int i = 0; i < OPNUM; i++){
    temp += bundled_sound[i];
  }

  //pthread_mutex_unlock(&(calsa_lock)); //TODO: really think about where these locks should go;
  //printf("%f\n", temp);
  //printf("5");
  //Sound Math
  //std::cout << temp<< std::endl;
  float pre_s = vol*temp/10; // sound is divided by arbitrary number here, probably should remove that
  //printf("%f\n", pre_s);
  if (pre_s > 50000){ //Arbitrary clipping point
    pre_s = 50000;
  }
  if(pre_s < 0){
    pre_s = 0;
  }
  s[0] = (unsigned short)(pre_s);
  s[1] = s[0];
  return s;
}


//TODO: look through code and see if there is anywhere you can cut runtime.  AKA THIS IS TOO SLOW RIGHT NOW. PLZ FIX.