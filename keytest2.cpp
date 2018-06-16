#include <stdio.h>
#include <ncurses.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <map>
#include <cmath>
#include <thread>
#include <chrono>
#include <queue>

#define EV_PRESSED 1
#define EV_RELEASED 0
#define EV_REPEAT 2

#define MAXNOTES 8
#define BUFFERSIZE 128

const double R=8000; // sample rate (samples per second)
const double V=255; // a volume constant

std::unordered_set<int> held_keys = {};
std::map<int, double> code2freq = {{44, 261.625565}, {45, 293.66}, {46, 329.63}, {47, 349.23}, {48, 392.00}};

std::queue<uint8_t> buf;


void audio_writer()
{
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  double wait_time = (1/R*1e+9); //waittime for writing in nanoseconds
  long t = 0;
  while (1){
    int timer = (std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::steady_clock::now() - begin).count() - wait_time * t);
    //std::cout<<timer<<std::endl;
    if (!buf.empty() and timer > wait_time){
      std::cout << buf.front();
      buf.pop();
      t++;
    }
  }
}


void audio_producer()
{
  long t = 0;
  while(1)
  {
    if (buf.size() < BUFFERSIZE)
    {
      uint8_t sound = 0;
      double temp = 0;
      int i = 0;
      for (int const& keycode: held_keys)
      {
        try{
          double freq = code2freq.at(keycode);
          temp += (sin(t*2.0*M_PI/R*freq)+1)/2;
          //std::cout << temp<<std::endl;
          if (++i >= MAXNOTES) { break; }
        }catch(const std::out_of_range& oor){
          continue;
        }
      }
      if(i > 0){
        sound = (uint8_t) (temp * V/i);
        buf.push(sound);
        t++;
      }
    }
    //std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    //int nanos_passed = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
    //int nanos_left = (1/R)*1e+9 - nanos_passed;
    //std::this_thread::sleep_for(std::chrono::nanoseconds((int)((1/R*1e+9)));

  }
  return;
}

main()
{
  /************************************************
  * IMPORTANT
  * you need to execute this code as the su or
  * sudo user in order to open the device properly.
  ***********************************************/
  initscr();
  noecho();
  printf("Starting the keyboard musician \n");


  const char *device = "/dev/input/event4"; // This is the keyboard device as identified using both: $cat /proc/bus/input/devices
  
  int fd1 = 0;
  if( (fd1 = open(device, O_RDONLY)) > 0 )
  {
    std::thread audio_w (audio_writer); //Start audio loop
    std::thread audio_p (audio_producer); //Start audio loop
    while(1) // It's important to open a new file descriptor here or the program will block.
    {
      struct input_event event;
      read(fd1, &event, sizeof(struct input_event));

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

      if(event.value == EV_PRESSED) { held_keys.insert(event.code); }
      if(event.value == EV_RELEASED) { held_keys.erase(event.code); }

    }
    close(fd1);
  }
  //endwin()
}