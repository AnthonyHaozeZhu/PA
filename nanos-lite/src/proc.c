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
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // Log("run proc go to %x", entry);
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

_RegSet* schedule(_RegSet *prev) {
  if(current != NULL) {
    current -> tf = prev;
  }
  else {
    current = &pcb[0];
  }
  static int num = 0;
  static const int frequency = 1000;
  if(current == &pcb[0]) {
    num++;
  }
  else {
    current = &pcb[0];
  }
  if(num == frequency) {
    current = &pcb[1];
    num = 0;
  }
  // current = (current == &pcb[0]? &pcb[1] : &pcb[0]);
  // Log("ptr = 0x%x\n", (uint32_t)current -> as.ptr);
  _switch(&current -> as);
  return current -> tf;
}
