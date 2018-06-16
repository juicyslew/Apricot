

#ifndef CALSA_PLAYER
#define CALSA_PLAYER 1

#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <alsa/asoundlib.h>
#include <string>
#include <pthread.h>
#include <time.h>
#include "global_definitions.h"
#include "calsa.h"


#ifdef __cplusplus
extern "C" {
#endif

  struct player_st {
    /* Handle for the PCM device */ 
    snd_pcm_t *pcm_handle;          

    /* Playback stream */
    snd_pcm_stream_t stream;// = SND_PCM_STREAM_PLAYBACK;

    /* This structure contains information about    */
    /* the hardware and can be used to specify the  */      
    /* configuration to be used for the PCM stream. */ 
    snd_pcm_hw_params_t *hwparams;

    /* Name of the PCM device, like plughw:0,0          */
    /* The first number is the number of the soundcard, */
    /* the second number is the number of the device.   */
    char *pcm_name;

    unsigned int exact_rate;   /* Sample s_rate returned by */
                      /* snd_pcm_hw_params_set_rate_near */ 
    int dir;          /* exact_rate == s_rate --> dir = 0 */
                      /* exact_rate < s_rate  --> dir = -1 */
                      /* exact_rate > s_rate  --> dir = 1 */
    int periods;// = 2;       /* Number of periods */
    snd_pcm_uframes_t periodsize;// = 8192; /* Periodsize (bytes) */

    calsa_t calsa;


    player_st(){

    }
  };

  //Initialize sound player
  player_t player_init(void);

  //Test Simple Sound Generation
  void sine_test(player_t player, float freq);

  void *start_audio(void *p_player);


#ifdef __cplusplus
}
#endif

#endif