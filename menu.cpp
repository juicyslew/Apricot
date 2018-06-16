
#include "menu.h"
//MENU INITIALIZATION


//General Structures
std::vector<std::string> waveform_string{
  "sine",
  "saw",
  "square",
  "noise"
};

void menu_init(calsa_t calsa){

  //INITIALIZE MENU OBJECTS

  //std::list<menu_t> menulist;
  menu_t initial_menu = new menu_st(); //(menu_t)calloc(1, sizeof(struct menu_st));



  //##################### INITIAL MENU #####################//
  //########################################################//
  std::vector<std::string> settingHeaders_init;
  std::vector<void *> set_vars_init;
  for (int i = 0; i < OPNUM; i++){
    settingHeaders_init.push_back("Operator " + std::to_string(i));
  }

  initial_menu->settingHeaders = settingHeaders_init;
  initial_menu->set_vars = set_vars_init;

  std::vector<enum settingStyle> Styles_init{
    calsa_link,
    calsa_link,
    calsa_link,
    calsa_link,
    calsa_link,
    calsa_link,
  };

  initial_menu->Styles = Styles_init;


  std::vector<int> settingVals_init
  {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  };

  initial_menu->settingVals = settingVals_init;
  initial_menu->title = "CALSA CONTROLLER";

  //UpdateSettings(initial_menu);


  for (int i = 0; i < OPNUM; i++){
    //op menu
    menu_t tmp_op_menu = op_menu_init();
    //printf("here");
    //memcpy(tmp_op_menu, op_menu, sizeof(struct menu_st));
    //tmp_op_menu->title = (tmp_op_menu->title + std::to_string(i)).c_str();

    //printf("here 1");


    //venv menu
    //menu_t tmp_venv_menu = env_menu_init();
    //tmp_venv_menu->title = (tmp_venv_menu->title + std::to_string(i)).c_str();

    //printf("here 2");

    /*tmp_venv_menu->set_vars.push_back(&(calsa->optors[i]->volenv->start_val));
    tmp_venv_menu->set_vars.push_back(&(calsa->optors[i]->volenv->attack_time));
    tmp_venv_menu->set_vars.push_back(&(calsa->optors[i]->volenv->attack_val));
    tmp_venv_menu->set_vars.push_back(&(calsa->optors[i]->volenv->decay_time));
    tmp_venv_menu->set_vars.push_back(&(calsa->optors[i]->volenv->sustain_val));
    tmp_venv_menu->set_vars.push_back(&(calsa->optors[i]->volenv->release_time));
    tmp_venv_menu->set_vars.push_back(&(calsa->optors[i]->volenv->release_val));
    tmp_venv_menu->set_vars.push_back(tmp_op_menu);
    tmp_venv_menu->escape_to = tmp_op_menu;*/


    //printf("here 3");


    //mod menu
    menu_t tmp_mod_menu = mod_menu_init();
    //tmp_mod_menu->title = (tmp_mod_menu->title + std::to_string(i)).c_str();

    for (int j = 0; j < OPNUM; j++){
      tmp_mod_menu->settingHeaders.push_back("Receive Op " + std::to_string(j));
      tmp_mod_menu->set_vars.push_back(&(calsa->optors[i]->modreceive[j]));
    }
    tmp_mod_menu->settingHeaders.push_back("Back");
    tmp_mod_menu->set_vars.push_back(tmp_op_menu);
    tmp_mod_menu->escape_to = tmp_op_menu;


    //printf("here 4");


    tmp_op_menu->set_vars.push_back(&(calsa->optors[i]->style));
    //tmp_op_menu->set_vars.push_back(tmp_venv_menu);
    tmp_op_menu->set_vars.push_back(tmp_mod_menu);
    tmp_op_menu->set_vars.push_back(&(calsa->optors[i]->volstatic));
    tmp_op_menu->set_vars.push_back(initial_menu);
    tmp_op_menu->escape_to = initial_menu;
    initial_menu->set_vars.push_back(tmp_op_menu);
    UpdateSettings(tmp_op_menu);
    UpdateSettings(tmp_mod_menu);
    //UpdateSettings(tmp_venv_menu);
  }
  UpdateSettings(initial_menu);


  //UpdateSettings(venv_menu);

  curr_menu = initial_menu;
}

menu_t op_menu_init(void){
  menu_t op_menu = (menu_t)calloc(1, sizeof(struct menu_st));
  
  //##################### OPERATOR MENU #####################//
  //#########################################################//
  std::vector<std::string> settingHeaders_op{
    "waveform",
    //"VolEnv",
    "ModReceive",
    "OutVol",
    "Back",
  };
  op_menu->settingHeaders = settingHeaders_op;

  std::vector<enum settingStyle> Styles_op{
    calsa_list,
    //calsa_link,
    calsa_link,
    calsa_bar,
    calsa_link
  };
  op_menu->Styles = Styles_op;
  std::vector<void *> set_vars_op;
  op_menu->set_vars = set_vars_op;


  std::vector<int> settingVals_op
  {
    0,
    //NULL,
    NULL,
    0,
    NULL,
  };
  op_menu->settingVals = settingVals_op;
  op_menu->title = "OPERATOR ";

  op_menu->setting_lists.push_back(waveform_string);
  return op_menu;
}

/*menu_t env_menu_init(void){
  menu_t env_menu = (menu_t)calloc(1, sizeof(struct menu_st));

  //##################### VOLUME_ENV MENU #####################//
  //###########################################################//
  std::vector<std::string> settingHeaders_env{
    "StartVol",
    "AttackTime",
    "AttackVol",
    "DecayTime",
    "SustainVol",
    "ReleaseTime",
    "ReleaseVol",
    "Back",
  };
  env_menu->settingHeaders = settingHeaders_env;

  std::vector<enum settingStyle> Styles_env{
    calsa_bar,
    calsa_bar,
    calsa_bar,
    calsa_bar,
    calsa_bar,
    calsa_bar,
    calsa_bar,
    calsa_link,
  };
  env_menu->Styles = Styles_env;

  std::vector<void *> set_vars_env;

  env_menu->set_vars = set_vars_env;


  std::vector<int> settingVals_env
  {
    0,
    1,
    30,
    5,
    24,
    30,
    0,
    NULL,
  };
  env_menu->settingVals = settingVals_env;
  env_menu->title = "VOLENV - OPERATOR ";
  return env_menu;
}*/

menu_t mod_menu_init(void){
  menu_t mod_menu = (menu_t)calloc(1, sizeof(struct menu_st));


  //##################### MOD MENU #####################//
  //####################################################//
  std::vector<std::string> settingHeaders_mod{
  };
  mod_menu->settingHeaders = settingHeaders_mod;

  std::vector<enum settingStyle> Styles_mod{
    calsa_bar,
    calsa_bar,
    calsa_bar,
    calsa_bar,
    calsa_bar,
    calsa_bar,
    calsa_link,
  };
  mod_menu->Styles = Styles_mod;

  std::vector<void *> set_vars_mod = {};
  mod_menu->set_vars = set_vars_mod;


  std::vector<int> settingVals_mod
  {
    0,
    0,
    0,
    0,
    0,
    0,
    NULL,
  };
  mod_menu->settingVals = settingVals_mod;
  mod_menu->title = "MODULATION - OPERATOR ";
  return mod_menu;
}


void * start_menu (void){

  int startx = 0;
  int starty = 0;

  WINDOW *menu_win;
  int highlight = 1;
  int c;

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
  print_menu(curr_menu, menu_win, highlight);
  int n_settings; 

  while(1)
  { 
    n_settings = curr_menu->settingVals.size();
    c = wgetch(menu_win);
    int did_something = 0;

    switch(c)
    {

      case KEY_UP:
        did_something = 1;
        if(highlight == 1)
          highlight = n_settings;
        else
          --highlight;
        break;
      case KEY_DOWN:
        did_something = 1;
        if(highlight == n_settings)
          highlight = 1;
        else 
          ++highlight;
        break;
      case KEY_LEFT:
        if (curr_menu->Styles[highlight-1] == calsa_list or curr_menu->Styles[highlight-1] == calsa_bar){
          curr_menu->settingVals[highlight-1] -= 1;
          did_something = 2;
        }
        break;
      case KEY_RIGHT:
        if (curr_menu->Styles[highlight-1] == calsa_list or curr_menu->Styles[highlight-1] == calsa_bar){
          curr_menu->settingVals[highlight-1] += 1;
          did_something = 2;
        }
        break;
      case 10: //KEY_ENTER
        if (curr_menu->Styles[highlight-1] == calsa_link){
          curr_menu = (menu_t)curr_menu->set_vars[highlight-1];
          did_something = 2;
          highlight = 1;
        }
        break;
      case 127: //backspace
        if (curr_menu->escape_to){
          curr_menu = (menu_t)curr_menu->escape_to;
          did_something = 2;
          highlight = 1;
        }
        break;
    }
    if (did_something > 0){
      print_menu(curr_menu, menu_win, highlight);
      if (did_something > 1){ UpdateSettings(curr_menu);}
    }
  } 

  
  endwin();
  pthread_mutex_destroy(&(calsa_lock));
  return NULL;
}

void UpdateSettings(menu_t menu){
  int n_settings = menu->settingVals.size();
  pthread_mutex_lock(&(calsa_lock));
  for(int i = 0; i < n_settings; i++){
    switch (menu->Styles[i]){
      case calsa_list:
      {
        /*printf("%s\n", menu->title);
        printf("%s\n", menu->title);
        printf("%s\n", menu->title);
        printf("%s\n", menu->title);*/
        int * var = (int *) menu->set_vars[i];
        *var = menu->settingVals[i];
        break;
      }
      case calsa_bar:
      {
        float * var = (float *) menu->set_vars[i];
        *var = (float)menu->settingVals[i] / BARLEN;
        break;
      }
      case calsa_str:
      {
        break;
      }
      case calsa_link:
      {
        break;
      }
      default:
        break;
    }
  }
  pthread_mutex_unlock(&(calsa_lock));
}


void print_menu(menu_t menu, WINDOW *menu_win, int highlight)
{
  int x, y, i;  

  x = 2;
  y = 2;
  wclear(menu_win);
  box(menu_win, 0, 0);
  int l = 0; //list counter
  for(i = 0; i < (signed int)menu->settingHeaders.size(); ++i)
  {
    std::string pline = menu->settingHeaders[i];
    for (int k = menu->settingHeaders[i].length(); k < HEADLEN; k++){
      if (k % 6 == 0){
        pline += ".";
      }else{
        pline += " ";
      }
    }
    switch (menu->Styles[i]){
      case calsa_list:
        {
          std::vector<std::string> myopts = menu->setting_lists[l];
          int size = myopts.size();
          if (menu->settingVals[i] >= size){
            menu->settingVals[i] = size-1;
          }
          if (menu->settingVals[i] < 0){
            menu->settingVals[i] = 0;
          }

          pline += myopts.at(menu->settingVals[i]);

          l++;
          //pline += std::to_string(settingVals[i]);
        }
        break;
      case calsa_bar:
        {
          //std::string fill(settingVals[i], '#');
          //std::string empty(BARLEN - settingVals[i], ' ');
          if (menu->settingVals[i] < 0){
            menu->settingVals[i] = 0;
          }
          if (menu->settingVals[i] > BARLEN){
            menu->settingVals[i] = BARLEN;
          }
          int j = 0;
          
          pline += "[";
          for ( ; j < menu->settingVals[i]; j++){
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
      case calsa_str:
        break;
        //do nothing for case string
      case calsa_link:
        pline += "=>";
        break;
      default:
        pline += "SETTING STYLE UNDEFINED";
        break;
    }
    const char * cstr = pline.data();
    //std::string pline = settingHeaders[i] + ", test";
    if(highlight == i + 1) /* High light the present choice */
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
  mvwprintw(menu_win, HEIGHT-2, 2, "%s", menu->title);
  wrefresh(menu_win);
}