#include <alsa/asoundlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <pthread.h>
#include "hashset.h"
#include "hashset_itr.h"
#include <math.h>


#define EV_PRESSED 1
#define EV_RELEASED 0
#define EV_REPEAT 2

#define MAXNOTES 1

/*------------ SETUP ------------*/

/* Handle for the PCM device */ 
snd_pcm_t *pcm_handle;          

/* Playback stream */
snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

/* This structure contains information about    */
/* the hardware and can be used to specify the  */      
/* configuration to be used for the PCM stream. */ 
snd_pcm_hw_params_t *hwparams;

/* Name of the PCM device, like plughw:0,0          */
/* The first number is the number of the soundcard, */
/* the second number is the number of the device.   */
char *pcm_name;

unsigned int rate = 44100; /* Sample rate */
unsigned int exact_rate;   /* Sample rate returned by */
                  /* snd_pcm_hw_params_set_rate_near */ 
int dir;          /* exact_rate == rate --> dir = 0 */
                  /* exact_rate < rate  --> dir = -1 */
                  /* exact_rate > rate  --> dir = 1 */
int periods = 2;       /* Number of periods */
snd_pcm_uframes_t periodsize = 8192; /* Periodsize (bytes) */

//bool held_keys[32]; //zxcvbnm,sdghjwertyui23567op90[=]
hashset_t held_keys;

const char *device = "/dev/input/event4";



void *key_detection(void *vargp){ //I don't get this syntax, look it up
  int fd;
  if( (fd = open(device, O_RDONLY)) > 0 )
  {
    while(1) // It's important to open a new file descriptor here or the program will block.
    { 
      struct input_event event;
      read(fd, &event, sizeof(struct input_event));

      /*std::cout << "{ ";
      for (intz const& i: held_keys)
      {
        std::cout << i << ", ";
      }
      std::cout << "}" << std::endl;*/

      if(event.type != EV_KEY)
      {
        continue;
      }

      if(event.value == EV_PRESSED) {
        //printf("2");
        int *newp = malloc(sizeof(int*));
        *newp = event.code;
        printf("%p :: ", newp);
        printf("%d\n", *newp);
        //hashset_add(held_keys, &event.code);
      }
      //printf("3");
      if(event.value == EV_RELEASED) {
        //printf("4"); 
        int *newp = malloc(sizeof(int*));
        *newp = event.code;
        printf("%p :: ", newp);
        printf("%d\n", *newp);
        //hashset_remove(held_keys, &event.code);
      }
      //printf("5");

    }
    close(fd);
  }
  return NULL;
}



int main(){


  //held_keys = hashset_create();
  //int x = 440;
  //hashset_add(held_keys, &x);

  /*------------ INITIALIZATION ------------*/

  /* Init pcm_name. Of course, later you */
  /* will make this configurable ;-)     */
  pcm_name = strdup("plughw:0,0");


  /* Allocate the snd_pcm_hw_params_t structure on the stack. */
  snd_pcm_hw_params_alloca(&hwparams);

  /* Open PCM. The last parameter of this function is the mode. */
  /* If this is set to 0, the standard mode is used. Possible   */
  /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */ 
  /* If SND_PCM_NONBLOCK is used, read / write access to the    */
  /* PCM device will return immediately. If SND_PCM_ASYNC is    */
  /* specified, SIGIO will be emitted whenever a period has     */
  /* been completely processed by the soundcard.                */
  if (snd_pcm_open(&pcm_handle, pcm_name, stream, 0) < 0) {
    fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
    return(-1);
  }

  /* Init hwparams with full configuration space */
  if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
    fprintf(stderr, "Can not configure this PCM device.\n");
    return(-1);
  }




  /*------------ CONFIGURE PCM ------------*/

  /* Set access type. This can be either    */
  /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
  /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
  /* There are also access types for MMAPed */
  /* access, but this is beyond the scope   */
  /* of this introduction.                  */
  if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
    fprintf(stderr, "Error setting access.\n");
    return(-1);
  }

  /* Set sample format */
  if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
    fprintf(stderr, "Error setting format.\n");
    return(-1);
  }

  /* Set sample rate. If the exact rate is not supported */
  /* by the hardware, use nearest possible rate.         */ 
  exact_rate = rate;
  if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, (unsigned int)0) < 0) {
    fprintf(stderr, "Error setting rate.\n");
    return(-1);
  }
  if (rate != exact_rate) {
    fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n" 
                     "==> Using %d Hz instead.\n", rate, exact_rate);
  }

  /* Set number of channels */
  if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
    fprintf(stderr, "Error setting channels.\n");
    return(-1);
  }

  /* Set number of periods. Periods used to be called fragments. */ 
  if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
    fprintf(stderr, "Error setting periods.\n");
    return(-1);
  }


  /* Set buffer size (in frames). The resulting latency is given by */
  /* latency = periodsize * periods / (rate * bytes_per_frame)     */
  if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (periodsize * periods)>>2) < 0) {
    fprintf(stderr, "Error setting buffersize.\n");
    return(-1);
  }


  /* Apply HW parameter settings to */
  /* PCM device and prepare device  */
  if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
    fprintf(stderr, "Error setting HW params.\n");
    return(-1);
  }

  //pthread_t thread_id;
  //pthread_create(&thread_id, NULL, key_detection, NULL);



  /*------------ START WRITING DATA ------------*/

  /* Write num_frames frames from buffer data to    */ 
  /* the PCM device pointed to by pcm_handle.       */
  /* Returns the number of frames actually written. */

  int num_frames;
  unsigned char *data;
  int pcmreturn, l1, l2;
  //short s1, s2;
  unsigned short s = 500;

  //int frames;
  const float m = 2.0*M_PI/rate;
  const float vol = 32767.0/2.0; // the /2.0 is important
  long t = 0;
  //double freq;
  //hashset_itr_t hash_iter;
  //hash_iter = hashset_iterator(held_keys);



  data = (unsigned char *)malloc(periodsize);
  num_frames = periodsize >> 2;
  printf("wowow: %d\n", num_frames);
  //printf("0");
  for(l1 = 0; l1 < 100; l1++) {
    for(l2 = 0; l2 < num_frames; l2++) {
      //printf("1");
      //ouble temp = 0;
      int i = 1;
      double temp = (sin(t * m * 1760)+1.0);
      /*if (held_keys->nitems != 0){
        hash_iter->index = 0; //reset iterator
        while (i < MAXNOTES)
        {
          int *p = (int *)hashset_iterator_value(hash_iter);
          if (!p){
            break;
          }
          temp += (sin(t * m * (*p))+1.0);
          i++;
          hashset_iterator_next(hash_iter);
        }

      }*/


      //printf("5");
      //Sound Math
      s = (unsigned short)(temp*(vol/i));

      data[4*l2] = (unsigned char)s & 0xff;
      data[4*l2+1] = (unsigned char)(s >> 8) & 0xff;
      data[4*l2+2] = (unsigned char)s & 0xff;
      data[4*l2+3] = (unsigned char)(s >> 8) & 0xff;
      t++;
      //printf("6");
    }
    //printf("7");
    printf("%d\n", s);
    while ((pcmreturn = snd_pcm_writei(pcm_handle, data, num_frames)) < 0) {
      snd_pcm_prepare(pcm_handle);
      fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
    }
  }

}