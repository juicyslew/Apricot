
#include "note.h"

note_t note_init(calsa_t controller, float freq, long long t){
  note_t note = new note_st();//(note_t)calloc(1, sizeof(struct note_st));
  //note->held = 1;
  note->freq = freq;
  note->press_t = t;
  note->release_t = 0;
  note->end_t = 0;
  note->volenv = (envgen_t*)malloc(OPNUM * sizeof(envgen_t));
  note->freqenv = (envgen_t*)malloc(OPNUM * sizeof(envgen_t));
  for (int i = 0; i < OPNUM; i++){
    note->volenv[i] = init_envelope();
    note->freqenv[i] = init_envelope();
  }
    

  //set rels
  //note->volrels = new float[OPNUM](); //(float *)calloc(OPNUM, sizeof(float));
  note->op_outs = new float[OPNUM](); //(float *)calloc(OPNUM, sizeof(float));
  note->phaseincs = new float[OPNUM]();

  //note->style = controller->style;
  //note->vol_attack = controller->vol_attack;
  //note->vol_release = controller->vol_release;
  //note->vol_max = controller->vol;

  //note->s_rate = s_rate;
  return note;
}

void calsa_read_notes(calsa_t calsa, long long t){
  //std::deque<struct ev_key>::iterator it = calsa->key_events.begin();
  for (struct ev_key ev : calsa->key_events){
    if (ev.type == EV_PRESSED){
      note_t note = note_init(calsa, code2freq.at(ev.code), t);
      calsa->active_notes[ev.code] = note;
    }
    if (ev.type == EV_RELEASED){
      if (!calsa->active_notes.count(ev.code)){
        continue;
      }
      note_t note = calsa->active_notes.at(ev.code);
      /*try
      {
        
      }catch (const std::out_of_range& oor){
        continue;
      }*/
      if (note){
        //RELEASE NOTE
        calsa->active_notes.erase(ev.code);
        note->release_t = t;
        // call optors env thing here
        for (int o = 0; o < OPNUM; o++){
          //note->volrels[o] = use_envelope(calsa->optors[o]->volenv, note, 0, 1, t);
          long long tmp = (long long)calsa->optors[o]->volinfo->release_time; //calsa->optors[o]->volenv->release_time * 3 * s_rate; //WARNING: MAKE SURE TO GET RID OF THIS 
          set_release_expo(note->volenv[o], calsa->optors[o]->volinfo);
          set_release_linear(note->freqenv[o], calsa->optors[o]->freqinfo);
          if (tmp > note->end_t){
            note->end_t = t+tmp;
          }
        }
        calsa->passive_notes.push_back(note);
      }
    }
    //it++;
  }
  calsa->key_events.clear();
}

void calsa_note_trimming(calsa_t calsa, long long t){
  //iterate through passive_notes and check for dead notes
  //printf("wowwie");
  //std::list<note_t>::iterator it = calsa->passive_notes.begin();
  for(auto it = calsa->passive_notes.begin(); it != calsa->passive_notes.end(); ){
    note_t tmpnote = *it;

    if (t > tmpnote->end_t){
      //free(tmpnote);
      it = calsa->passive_notes.erase(it);
      continue;
    }
    it++;
  }
}