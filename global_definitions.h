
#ifndef CALSA_GLOBALS
#define CALSA_GLOBALS 1

#include <vector>
#include <pthread.h>
#include <map>

#define EV_PRESSED 1
#define EV_RELEASED 0
#define EV_REPEAT 2

#define WIDTH 60
#define HEIGHT 20

#define BARLEN 30
#define HEADLEN 12

#define MAXNOTES 8
#define MAXSOUNDS 16

#define OPNUM 6

#define DBLOW 40



enum waveform{sine = 0, saw = 1, square = 2, noise = 3};
enum settingStyle {calsa_list, calsa_bar, calsa_str, calsa_link};
enum modmode{fm = 0, rm = 1};
enum envmode{linear = 0, exponential = 1};
enum envstate{START = 0, ATTACK = 1, DECAY = 2, SUSTAIN = 3, RELEASE = 4};

struct player_st;
typedef struct player_st *player_t;
struct ev_key;
struct calsa_st;
typedef struct calsa_st *calsa_t;
struct note_st;
typedef struct note_st *note_t;
struct menu_st;
typedef struct menu_st *menu_t;
struct operator_st;
typedef struct operator_st * operator_t;
struct envgen_st;
typedef struct envgen_st * envgen_t;
struct envinfo_st;
typedef struct envinfo_st * envinfo_t;


extern pthread_mutex_t calsa_lock;
extern menu_t curr_menu;
extern enum modmode fmrm;
extern unsigned int s_rate;
extern std::map<int, float> code2freq;


#endif