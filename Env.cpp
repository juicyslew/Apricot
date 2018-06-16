#include "Env.h"

const float min_val = 1e-6;

envgen_t init_envelope(){
  envgen_t env = new envgen_st();//(envgen_t)calloc(1, sizeof(struct envgen_st));
  env->state = START;
  env->timer = -1;
  env->interval = 0;
  env->val = min_val;
  /*env->relvals =  (float *) calloc(MAXSOUNDS, sizeof(float));
  for (int i = 0; i < MAXSOUNDS; i++){
    env->relvals[i] = 0.0;
  }*/
  return env;
}

envinfo_t init_envelope_info(){
  envinfo_t info = new envinfo_st();//(envgen_t)calloc(1, sizeof(struct envgen_st));

  info->attack_time = 0 * s_rate;
  info->attack_val = 0 + min_val;

  info->decay_time = 0 * s_rate;

  info->sustain_val = 1 + min_val;

  info->release_time = 4 * s_rate;
  info->release_val = 0 + min_val;
  return info;
}


void set_release_expo(envgen_t env, envinfo_t info){
  env->state = RELEASE;
  env->timer = info->release_time;
  env->interval = compute_interval_expo(info->release_val, env);
}

void set_release_linear(envgen_t env, envinfo_t info){
  env->state = RELEASE;
  env->timer = info->release_time;
  env->interval = compute_interval_linear(info->release_val, env);
}


float use_envelope_expo(envgen_t env, envinfo_t info){
  // i is the index of the note in the list it's in;
  //EVERYTHING IS LINEAR FOR NOW
  //printf("%d\n", note->held);
  //float val = 0;
  while(env->timer <= 0){
    switch(env->state){
      case START:
        env->state = ATTACK;
        env->timer = (long long) info->attack_time;
        env->interval = compute_interval_expo(info->attack_val, env);
        break;

      case ATTACK:
        env->state = DECAY;
        env->timer = (long long) info->decay_time;
        env->interval = compute_interval_expo(info->sustain_val, env);
        break;
      case DECAY:
        env->state = SUSTAIN;
        env->timer = std::numeric_limits<long long>::max();
        env->interval = 1;//compute_interval(env->sustain_val, env->val, env->timer);
        break;
      case SUSTAIN:
        env->timer = std::numeric_limits<long long>::max();
        env->interval = 1;
        break;
      case RELEASE:
        return 0;
    }
  }

  env->val *= env->interval;

  //printf("%f\n", val);
  env->timer--;
  return env->val;
}

float use_envelope_linear(envgen_t env, envinfo_t info){
  // i is the index of the note in the list it's in;
  //EVERYTHING IS LINEAR FOR NOW
  //printf("%d\n", note->held);
  //float val = 0;
  while (env->timer <= 0){
    switch(env->state){
      case START:
        env->state = ATTACK;
        env->timer = (long long) info->attack_time;
        env->interval = compute_interval_linear(info->attack_val, env);
        break;

      case ATTACK:
        env->state = DECAY;
        env->timer = (long long) info->decay_time;
        env->interval = compute_interval_linear(info->sustain_val, env);
        break;
      case DECAY:
        env->state = SUSTAIN;
        env->timer = std::numeric_limits<long long>::max();
        env->interval = 0;//compute_interval(env->sustain_val, env->val, env->timer);
        break;
      case SUSTAIN:
        env->timer = std::numeric_limits<long long>::max();
        env->interval = 0;
        break;
      case RELEASE:
        return 0;
    }
  }

  env->val += env->interval;

  //printf("%f\n", val);
  env->timer--;
  return env->val;
}


inline double compute_interval_expo(double nextlevel, envgen_t env)
{
  if (env->timer <= 0) { env->val = nextlevel; return 1; }
  return std::pow(nextlevel/env->val, 1.0/env->timer);
}
inline double compute_interval_linear(double nextlevel, envgen_t env){
  if (env->timer <= 0) { env->val = nextlevel; return 0; }
  return (nextlevel-env->val)/env->timer;
}