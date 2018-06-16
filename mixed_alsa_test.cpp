#include <stdio.h>
#include <iostream>
#include <linux/input.h>
#include <pthread.h>
//#include <unordered_set>
#include <map>
#include <ncurses.h>
#include <vector>
#include <math.h>
#include <deque>
#include "global_definitions.h"
#include "note.h"
#include "calsa.h"
#include "player.h"
#include "menu.h"




const char *device = "/dev/input/event4";

//initialize some global variables
pthread_mutex_t calsa_lock;
menu_t curr_menu;
enum modmode fmrm = fm;
unsigned int s_rate = 44100;


//std::deque<struct input_event> key_events;
std::map<int, float> code2freq = {
       {3 , 554.37}, {4 , 622.25},               {6 , 739.99}, {7 , 830.61}, {8 , 932.33},              {10, 1108.73}, {11, 1244.51},
{16, 523.25}, {17, 587.33}, {18, 659.25}, {19, 698.46}, {20, 783.99}, {21, 880.00}, {22, 987.77}, {23, 1046.50}, {24, 1174.66}, {25, 1318.51},
       {31, 277.18}, {32, 311.13},               {34, 369.99}, {35, 415.30}, {36, 466.16},              {38, 554.37}, {39, 622.25},
{44, 261.63}, {45, 293.66}, {46, 329.63}, {47, 349.23}, {48, 392.00}, {49, 440.00}, {50, 493.88}, {51, 523.25}, {52, 587.33}, {53, 659.25}};




//int n_settings = sizeof(settingHeaders) / sizeof(std::string);
//void print_menu(WINDOW *menu_win, calsa_t controller, int highlight);
void *KeyDetection(void *p_controller);
//void UpdateSettings(calsa_t controller, const void * set_vars[]);


int main(){
  player_t player = player_init();
  if (!player){ return 1; }

  //menu_init(controller);

  if (pthread_mutex_init(&(calsa_lock), NULL) != 0)
  {
      printf("\n mutex init has failed\n");
      return 1;
  }

  //start threads
  pthread_t t1, t2;
  int err;

  /*err = pthread_create(&t1, NULL, start_audio, &player);
  if (err != 0){
    printf("\nThread can't be created :[%s]", strerror(err));
  }*/

  err = pthread_create(&t2, NULL, KeyDetection, &(player->calsa));
  if (err != 0){
    printf("\nThread can't be created :[%s]", strerror(err));
  }
  start_audio(&player);
  //start_menu();

  //start_audio(player);


  return 0;
}

void *KeyDetection(void *p_controller){
  calsa_t controller = *(calsa_t *)p_controller;
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

      if(event.value == EV_PRESSED || event.value == EV_RELEASED) { 
        if (!code2freq.count(event.code)){
          continue;
        }
        struct ev_key ev;
        ev.code = event.code;
        ev.type = event.value;
        controller->key_events.push_back(ev);  //TODO: make struct specifically for this to lessen amount of space saved.
        //controller->active_notes[event.code] = code2freq.at(keycode);
      }
      //pthread_mutex_unlock(&(calsa_lock));
    }
    close(fd);
  }
  return NULL;
}

/*void print_menu(WINDOW *menu_win, calsa_t controller, int highlight)
{
  int x, y, i;  

  x = 2;
  y = 2;
  wclear(menu_win);
  box(menu_win, 0, 0);
  int l = 0; //list counter
  for(i = 0; i < n_settings; ++i)
  {
    std::string pline = settingHeaders[i];
    for (int k = settingHeaders[i].length(); k < HEADLEN; k++){
      if (k % 6 == 0){
        pline += ".";
      }else{
        pline += " ";
      }
    }
    switch (Styles[i]){
      case list:
        {
          std::vector<std::string> mystyles = setting_lists[l];
          int size = mystyles.size();
          if (settingVals[i] >= size){
            settingVals[i] = size-1;
          }
          if (settingVals[i] < 0){
            settingVals[i] = 0;
          }

          pline += mystyles.at(settingVals[i]);

          l++;
          //pline += std::to_string(settingVals[i]);
        }
        break;
      case bar:
        {
          //std::string fill(settingVals[i], '#');
          //std::string empty(BARLEN - settingVals[i], ' ');
          if (settingVals[i] < 0){
            settingVals[i] = 0;
          }
          if (settingVals[i] > BARLEN){
            settingVals[i] = BARLEN;
          }
          int j = 0;
          
          pline += "[";
          for ( ; j < settingVals[i]; j++){
            if (j % 6 == 0){
              pline += "$";
            }else{
              pline += "#";
            }

          }
          for ( ; j < BARLEN; j++){
            if (j % 6 == 0){
              pline += ".";
            }else{
              pline += " ";
            }
          }
          pline += "]";
        }
        break;
      case str:
        break;
        //do nothing for case string
      default:
        pline += "SETTING STYLE UNDEFINED";
        break;
    }
    const char * cstr = pline.data();
    //std::string pline = settingHeaders[i] + ", test";
    if(highlight == i + 1) // High light the present choice
    {
      //move(y,0);
      //clrtoeol();
      wattron(menu_win, A_STANDOUT);
      mvwprintw(menu_win, y, x, "%s", cstr);
      wattroff(menu_win, A_STANDOUT);
    }
    else{
      //move(y,0);
      //clrtoeol();
      mvwprintw(menu_win, y, x, "%s", cstr);
    }
    ++y;
  }
  wrefresh(menu_win);
}*/