#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
 char str[32];
 int value;
 int hitnum;
} WP;

WP* new_wp();
bool free_wp(int NO);
bool check_point();
void show_point();

#endif
