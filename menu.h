

#ifndef CALSA_MENU
#define CALSA_MENU 1

#include <stdlib.h>
#include <string>
#include <ncurses.h>
#include <vector>
#include "global_definitions.h"
#include "calsa.h"

//Menus
struct menu_st{
  std::vector<enum settingStyle> Styles; //deep
  std::vector<std::vector<std::string>> setting_lists; //shallow
  std::vector<std::string> settingHeaders; //shallow
  std::vector<int> settingVals; //deep
  std::vector<void *> set_vars; //deep
  const char * title; //deep
  menu_t escape_to;

  menu_st(){

  }
};


void menu_init(calsa_t controller);

menu_t op_menu_init(void);

menu_t env_menu_init(void);

menu_t mod_menu_init(void);

void * start_menu(void);

void UpdateSettings(menu_t menu);

void print_menu(menu_t menu, WINDOW *menu_win, int highlight);


#endif