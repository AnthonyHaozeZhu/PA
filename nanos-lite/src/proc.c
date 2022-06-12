#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
//  _switch(&pcb[i].as);
 // current = &pcb[i];
 // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

static int current_game=0;

void change(){
 current_game=2-current_game;
}

static int i=0;
_RegSet* schedule(_RegSet *prev) {
  current->tf=prev;
  i++;
  // current=(i%200==0?&pcb[1]:&pcb[current_game]); 
 current=&pcb[0];
  _switch(&current->as);
  return current->tf;
}
