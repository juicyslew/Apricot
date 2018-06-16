#include <stdio.h>
#include <ncurses.h>
#include <iostream>
#include <cmath>
#include <stdint.h>
#include <map>
#include <thread>

#define WIDTH 30
#define HEIGHT 10
#define MAXNOTES 8

int startx = 0;
int starty = 0;

const double R=8000; // sample rate (samples per second)
//const double C=261.625565; // frequency of middle-C (hertz)
//double C = 200;
//double D = 240;
const double V=127; // a volume constant

std::map<int, double> code2freq = {{122, 261.625565}, {120, 293.66}, {99, 329.63}, {118, 349.23}, {98, 392.00}};

double freq = 0;


void audio_loop()
{
  for ( int t=0; ; t++ )
  {
    //uint8_t temp = (sin(t*2*M_PI/R*freq)+1)*V;
    //int vol = V / (sizeof(codes)/sizeof(codes[0]));
    /*for (int code : codes)
    {
      
      if (code == 0) { break; }
      std::cout << code;
      /*double freq = code2freq.at(code);
      temp += (sin(t*2*M_PI/R*freq)+1)*vol; // pure middle C sine wave
    }*/
    //uint8_t temp = t/F*C; // middle C saw wave (bytebeat style)
    //uint8_t temp = (t*5&t>>1)|(t*3&t>>1); // viznut bytebeat composition
    
  }
  return;
}



int main()
{ 

  //WINDOW *menu_win;
  /*int highlight = 1;
  int choice = 0;*/
  int c;

  initscr();
  noecho();
  cbreak();
  nodelay(stdscr, TRUE);
   /* Line buffering disabled. pass on everything */
  //startx = (80 - WIDTH) / 2;
  //starty = (24 - HEIGHT) / 2;
    
  //menu_win = newwin(HEIGHT, WIDTH, starty, startx);
  //keypad(menu_win, TRUE);

  //std::thread audio (audio_loop); //Start audio loop

  while(1)
  {
    while((c = getch()))
    {
      try
      {
        freq = code2freq.at(c);
        std::cout<< freq <<std::endl;
        //codes[p++] = c;
      }
      catch(const std::out_of_range& oor)
      {
        freq = 0;
      }
    }

    /*while(p < MAXNOTES)
    {
      codes[p++] = 0;
    }
    if (codes[0] != 0){
      std::cout << codes[0] << std::endl;
    }*/

    //audio_loop();
    clrtoeol();
    refresh();
    
    //print_menu(menu_win, highlight);
    //if(choice != 0) /* User did a choice come out of the infinite loop */
      //break;
  } 
  //mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
  clrtoeol();
  refresh();
  endwin();
  return 0;
}


/*void print_menu(WINDOW *menu_win, int highlight)
{
  int x, y, i;  

  x = 2;
  y = 2;
  box(menu_win, 0, 0);
  for(i = 0; i < n_choices; ++i)
  { if(highlight == i + 1) // High light the present choice
    { wattron(menu_win, A_REVERSE); 
      mvwprintw(menu_win, y, x, "%s", choices[i]);
      wattroff(menu_win, A_REVERSE);
    }
    else
      mvwprintw(menu_win, y, x, "%s", choices[i]);
    ++y;
  }
  wrefresh(menu_win);
}*/