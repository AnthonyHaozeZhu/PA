#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static int used_next; //用于记录在head中下一个使用的wp的index
static WP *wptemp;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].old = 0;
    wp_pool[i].hitNum = 0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
  used_next = 0;
}

/* TODO: Implement the functionality of watchpoint */
bool new_wp(char *args) {
  //从free链表中返回一个空闲监视点结构
  if(free_ == NULL) {
    //首先查看free链表是否存在，如果不存在则报错
    assert(0);
  }
  //记录取出的结构并更新链表
  WP* result = free_;
  free_ = free_ -> next;

  //设置新的wp相关信息
  result -> NO = used_next;
  used_next++; //记录索引信息
  result -> next = NULL; //从链表中取出
  strcpy(result -> e, args);
  result -> hitNum = 0; //初始化触发次数
  bool is_success;
  result -> old = expr(result -> e, &is_success); //计算旧的值
  if(is_success == false) {
    printf("error in new_wp; expression fault!\n");
    return false;
  }

  //对head链表进行更新
  wptemp = head;
  if(wptemp == NULL) {
    head = result;
  }
  else {
    while (wptemp -> next != NULL)
    {
      wptemp = wptemp -> next;
    }
    wptemp -> next = result;
  }
  printf("Success: set watchpoint %d, oldvalue = %d\n", result -> NO, result -> old);
  return true;
}

//删除监视点
bool free_wp(int num) {
  WP *chosen = NULL; //被选中删除的监视点
  if(head == NULL) {
    printf("no watch point now\n");
    return false;
  }
  if(head -> NO == num) {
    chosen = head;
    head = head -> next;
  }
  else {
    wptemp = head;
    while (wptemp != NULL && wptemp -> next != NULL)
    {
      /* code */
      if(wptemp -> next -> NO == num) {
        chosen = wptemp -> next;
        wptemp -> next = wptemp -> next -> next;
        break; 
      }
      wptemp = wptemp -> next;
    }
  }
  //删除后在free链表中进行添加
  if(chosen != NULL) {
    chosen -> next = free_;
    free_ = chosen;
    return true;
  }
  return false;
}

void print_wp() {
  if(head == NULL) {
    printf("no watchpoint now\n");
    return;
  }
  printf("watchpoint:\n");
  printf("NO.  expr    hitTimes\n");
  wptemp = head;
  while (wptemp != NULL)
  {
    printf("%d  %s    %d\n", wptemp -> NO, wptemp -> e, wptemp -> hitNum);
    wptemp = wptemp ->next;
  }
}

bool watch_wp() {
  bool is_success;
  int result;
  if(head == NULL) {
    return true;
  } 
  wptemp = head;
  while (wptemp != NULL)
  {
    /* code */
    result = expr(wptemp -> e, &is_success);
    if(result != wptemp -> old)
    {
      wptemp -> hitNum += 1;
      printf("Hardware watchpoint %d:%s\n", wptemp -> NO, wptemp -> e);
      printf("Old value:%d\nNew valus:%d\n\n", wptemp -> old, result);
      wptemp -> old = result;
      return false;
    }
    wptemp = wptemp -> next;
  }
  return true;
}


