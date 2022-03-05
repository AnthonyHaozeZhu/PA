#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  int old; //旧的值
  char e[32]; //表达式
  int hitNum; //记录触发次数

} WP;

bool new_wp(char *arg);
bool free_wp(int num);
void print_wp();
bool watch_wp();

#endif
