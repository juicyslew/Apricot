#include <stdio.h>
#include <iostream>
#include <linux/input.h>
#include <pthread.h>
#include <unordered_set>
#include <map>
#include <ncurses.h>
#include "calsa.h"


#define EV_PRESSED 1
#define EV_RELEASED 0
#define EV_REPEAT 2

#define WIDTH 60
#define HEIGHT 20


const char *device = "/dev/input/event4";

std::unordered_set<int> held_keys = {};
std::map<int, double> code2freq = {
       {3 , 554.37}, {4 , 622.25},               {6 , 739.99}, {7 , 830.61}, {8 , 932.33},              {10, 1108.73}, {11, 1244.51},
{16, 523.25}, {17, 587.33}, {18, 659.25}, {19, 698.46}, {20, 783.99}, {21, 880.00}, {22, 987.77}, {23, 1046.50}, {24, 1174.66}, {25, 1318.51},
       {31, 277.18}, {32, 311.13},               {34, 369.99}, {35, 415.30}, {36, 466.16},              {38, 554.37}, {39, 622.25},
{44, 261.63}, {45, 293.66}, {46, 329.63}, {47, 349.23}, {48, 392.00}, {49, 440.00}, {50, 493.88}, {51, 523.25}, {52, 587.33}, {53, 659.25}};


int startx = 0;
int starty = 0;

enum settingStyle {list, bar, str};

const enum settingStyle settingStyles[] =
{
  list,
  bar,
  bar,
  bar,
  str,
};

//ALL SETTINGS LISTS
const char ** waveform_string = malloc();


void *setting_lists[1];



const int bar_len = 24;
const int header_len = 12;

const char *settingHeaders[5];


int settingVals[] =
{
  0,
  0,
  0,
  0,
  0,
};


int n_settings = sizeof(settingHeaders) / sizeof(char *);
void print_menu(WINDOW *menu_win, calsa_t controller, int highlight);
void *KeyDetection(void *p_controller);


int main(){
  waveform_string[0] = "sine";
  waveform_string[1] = "saw";
  waveform_string[2] = "square";
  waveform_string[3] = "noise";

  settingHeaders[0] = "Waveform";
  settingHeaders[1] = "Attack";
  settingHeaders[2] = "Release";
  settingHeaders[3] = "Volume";
  settingHeaders[4] = "Exit";

  setting_lists[0] = &waveform_string;

  calsa_t controller = calsa_init();
  WINDOW *menu_win;
  int highlight = 1;
  int choice = 0;
  int c;

  if (!controller){
    return 1;
  }

  if (pthread_mutex_init(&(controller->lock), NULL) != 0)
  {
      printf("\n mutex init has failed\n");
      return 1;
  }

  //start threads
  pthread_t t1, t2;
  int err;

  err = pthread_create(&t1, NULL, start_audio, &controller);
  if (err != 0){
    printf("\nThread can't be created :[%s]", strerror(err));
  }

  err = pthread_create(&t2, NULL, KeyDetection, &controller);
  if (err != 0){
    printf("\nThread can't be created :[%s]", strerror(err));
  }

  initscr();
  clear();
  noecho();
  cbreak();
  startx = (80 - WIDTH) / 2;
  starty = (24 - HEIGHT) / 2;
  menu_win = newwin(HEIGHT, WIDTH, starty, startx);
  keypad(menu_win, TRUE);
  mvprintw(0, 0, "Use arrow keys select and set the settings of the synth.");
  refresh();
  print_menu(menu_win, controller, highlight);

  while(1)
  { 
    c = wgetch(menu_win);
    switch(c)
    { 
      case KEY_UP:
        if(highlight == 1)
          highlight = n_settings;
        else
          --highlight;
        break;
      case KEY_DOWN:
        if(highlight == n_settings)
          highlight = 1;
        else 
          ++highlight;
        break;
      case KEY_LEFT:
        settingVals[highlight-1] -= 1;
        break;
      case KEY_RIGHT:
        settingVals[highlight-1] += 1;
        break;
    }
    print_menu(menu_win, controller, highlight);
  } 

  
  endwin();
  pthread_mutex_destroy(&(controller->lock));
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

      if(event.value == EV_PRESSED) { 
        held_keys.insert(event.code);
      }
      if(event.value == EV_RELEASED) { held_keys.erase(event.code); }
      
      pthread_mutex_lock(&(controller->lock));
      int i = 0;
      for (int const& keycode: held_keys)
      {
        if (i >= MAXNOTES){
          break;
        }
        if (!code2freq.count(keycode)){
          continue;
        }
        controller->freqs[i] = code2freq.at(keycode);
        i++;
      }
      if (i < MAXNOTES){
        controller->freqs[i] = -1;
      }
      pthread_mutex_unlock(&(controller->lock));
    }
    close(fd);
  }
  return NULL;
}

void print_menu(WINDOW *menu_win, calsa_t controller, int highlight)
{
  int x, y, i;  

  x = 2;
  y = 2;
  box(menu_win, 0, 0);
  int l = 0; //list counter
  for(i = 0; i < n_settings; ++i)
  {
    char pline[100];
    strcpy(pline, settingHeaders[i]);//std::string pline = settingHeaders[i];
    printf("%lu\n", sizeof(*settingHeaders[i])/sizeof(char));
    for (int k = sizeof(settingHeaders[i])/sizeof(char); k < header_len; k++){
      if (k % 6 == 0){
        strcat(pline, ".");
      }else{
        strcat(pline, " ");
      }
    }
    switch (settingStyles[i]){
      case list:
        {
          //std::string *mylist = (std::string *)setting_lists[l];
          const char * mylist = (const char *)setting_lists[l];
          int size = sizeof(*mylist)/sizeof(char *);
          if (settingVals[i] >= size){
            settingVals[i] = size-1;
          }
          if (settingVals[i] < 0){
            settingVals[i] = 0;
          }

          strcat(pline, &mylist[settingVals[i]]);

          l++;
          //pline += std::to_string(settingVals[i]);
        }
        break;
      case bar:
        {
          //std::string fill(settingVals[i], '#');
          //std::string empty(bar_len - settingVals[i], ' ');
          int j = 0;
          
          strcat(pline, "[");
          for ( ; j < settingVals[i]; j++){
            if (j % 6 == 0){
              strcat(pline, "$");
            }else{
              strcat(pline, "#");
            }

          }
          for ( ; j < bar_len; j++){
            if (j % 6 == 0){
              strcat(pline, ".");
            }else{
              strcat(pline, " ");
            }
          }
          strcat(pline, "]");
        }
        break;
      case str:
        break;
        //do nothing for case string
      default:
        strcat(pline, "ERROR: STYLE UNDEFINED");
        break;
    }
    //std::string pline = settingHeaders[i] + ", test";
    const char *printline = (const char *)pline;
    if(highlight == i + 1) /* High light the present choice */
    { 
      wattron(menu_win, A_STANDOUT);
      mvwprintw(menu_win, y, x, "%s", printline);
      wattroff(menu_win, A_STANDOUT);
    }
    else
      mvwprintw(menu_win, y, x, "%s", printline);
    ++y;
  }
  wrefresh(menu_win);
}
