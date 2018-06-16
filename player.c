

#include "player.h"

player_t player_init(void){
  srand(time(NULL));   // should only be called once
  
  player_t player = new player_st();//(player_t)malloc(sizeof(struct player_st));


  /*------------ INITIALIZATION ------------*/

  player->stream = SND_PCM_STREAM_PLAYBACK;
  player->periods = 2;
  player->periodsize = 8192;
  //player->calsa = (calsa_t) malloc(sizeof(struct calsa_st));
  player->calsa = calsa_init();

  /* Init pcm_name. Of course, later you */
  /* will make this configurable ;-)     */
  player->pcm_name = strdup("plughw:0,0");


  /* Allocate the snd_pcm_hw_params_t structure on the stack. */
  snd_pcm_hw_params_alloca(&(player->hwparams));


  /* Open PCM. The last parameter of this function is the mode. */
  /* If this is set to 0, the standard mode is used. Possible   */
  /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */ 
  /* If SND_PCM_NONBLOCK is used, read / write access to the    */
  /* PCM device will return immediately. If SND_PCM_ASYNC is    */
  /* specified, SIGIO will be emitted whenever a period has     */
  /* been completely processed by the soundcard.                */
  if (snd_pcm_open(&(player->pcm_handle), player->pcm_name, player->stream, 0) < 0) {
    //fprintf(stderr, "%d\n", snd_pcm_open(&(player->pcm_handle), player->pcm_name, player->stream, 0));
    fprintf(stderr, "Error opening PCM device %s\n", player->pcm_name);
    return NULL;
  }

  /* Init hwparams with full configuration space */
  if (snd_pcm_hw_params_any(player->pcm_handle, player->hwparams) < 0) {
    fprintf(stderr, "Can not configure this PCM device.\n");
    return NULL;
  }




  /*------------ CONFIGURE PCM ------------*/

  /* Set access type. This can be either    */
  /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
  /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
  /* There are also access types for MMAPed */
  /* access, but this is beyond the scope   */
  /* of this introduction.                  */
  if (snd_pcm_hw_params_set_access(player->pcm_handle, player->hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
    fprintf(stderr, "Error setting access.\n");
    return NULL;
  }

  /* Set sample format */
  if (snd_pcm_hw_params_set_format(player->pcm_handle, player->hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
    fprintf(stderr, "Error setting format.\n");
    return NULL;
  }

  /* Set sample s_rate. If the exact s_rate is not supported */
  /* by the hardware, use nearest possible s_rate.         */ 
  player->exact_rate = s_rate;
  if (snd_pcm_hw_params_set_rate_near(player->pcm_handle, player->hwparams, &(player->exact_rate), (unsigned int)0) < 0) {
    fprintf(stderr, "Error setting s_rate.\n");
    return NULL;
  }
  if (s_rate != player->exact_rate) {
    fprintf(stderr, "The s_rate %d Hz is not supported by your hardware.\n" 
                     "==> Using %d Hz instead.\n", s_rate, player->exact_rate);
  }

  /* Set number of channels */
  if (snd_pcm_hw_params_set_channels(player->pcm_handle, player->hwparams, 2) < 0) {
    fprintf(stderr, "Error setting channels.\n");
    return NULL;
  }

  /* Set number of periods. Periods used to be called fragments. */ 
  if (snd_pcm_hw_params_set_periods(player->pcm_handle, player->hwparams, player->periods, 0) < 0) {
    fprintf(stderr, "Error setting periods.\n");
    return NULL;
  }


  /* Set buffer size (in frames). The resulting latency is given by */
  /* latency = periodsize * periods / (s_rate * bytes_per_frame)     */
  if (snd_pcm_hw_params_set_buffer_size(player->pcm_handle, player->hwparams, (player->periodsize * player->periods)>>2) < 0) {
    fprintf(stderr, "Error setting buffersize.\n");
    return NULL;
  }


  /* Apply HW parameter settings to */
  /* PCM device and prepare device  */
  if (snd_pcm_hw_params(player->pcm_handle, player->hwparams) < 0) {
    fprintf(stderr, "Error setting HW params.\n");
    return NULL;
  }



  return player;
}


void sine_test(player_t player, float freq){
  int num_frames;
  unsigned char *data;
  int pcmreturn, l1, l2;
  //short s1, s2;
  unsigned short s;

  const float m = 2.0*M_PI/s_rate;
  const float vol = 32767.0/2.0; // the /2.0 is important
  long long t = 0;

  data = (unsigned char *)malloc(player->periodsize);
  num_frames = player->periodsize >> 2; //this is just dividing by 4
  float last = 0;
  float B = .2;

  for(l1 = 0; l1 < 100; l1++) {
    for(l2 = 0; l2 < num_frames; l2++) {
      //printf("1");
      float temp = (sin(t * m * freq + B * last)+1.0);
      last = temp;


      //printf("5");
      //Sound Math
      s = (unsigned short)(temp*vol);

      data[4*l2] = (unsigned char)s & 0xff;
      data[4*l2+1] = (unsigned char)(s >> 8) & 0xff;
      data[4*l2+2] = (unsigned char)s & 0xff;
      data[4*l2+3] = (unsigned char)(s >> 8) & 0xff;
      t++;
      //printf("6");
    }
    //printf("7");
    while ((pcmreturn = snd_pcm_writei(player->pcm_handle, data, num_frames)) < 0) {
      snd_pcm_prepare(player->pcm_handle);
      fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
    }
  }
}


void *start_audio(void *p_player){
  player_t player = *(player_t *)p_player;
  int num_frames;
  unsigned char *data;
  int pcmreturn, l2;
  //short s1, s2;
  unsigned short *s;
  //const float vol = 32726.0; // the /2.0 is important
  long long t = 0;

  data = (unsigned char *)malloc(player->periodsize);
  num_frames = player->periodsize >> 2; //this is just dividing by 4

  while(1){
    for(l2 = 0; l2 < num_frames; l2++) {
      s = get_samples(player->calsa, t);
      //printf("%hu\n", s);

      data[4*l2] = (unsigned char)s[0] & 0xff;
      data[4*l2+1] = (unsigned char)(s[0] >> 8) & 0xff;
      data[4*l2+2] = (unsigned char)s[1] & 0xff;
      data[4*l2+3] = (unsigned char)(s[1] >> 8) & 0xff;
      t++;
      //printf("6");
    }
    //printf("7");
    while ((pcmreturn = snd_pcm_writei(player->pcm_handle, data, num_frames)) < 0) {
      snd_pcm_prepare(player->pcm_handle);
      fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
    }
  }
  return NULL;
}


//TODO: look through code and see if there is anywhere you can cut runtime.  AKA THIS IS TOO SLOW RIGHT NOW. PLZ FIX.