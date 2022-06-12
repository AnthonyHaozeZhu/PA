#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <stdlib.h>

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void show_point(){

 printf("%s\t%s\t%s\t%s\t\n","Num","Type     ","What       ","hitTimes");
 WP *temp;
 temp=head;
 while(temp!=NULL){
 printf("%d\t%s\t%s\t%d\t\n",temp->NO,"Watchpoint",temp->str,temp->hitnum);

 temp=temp->next;
 }

}


void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP * new_wp(){

WP*temp;
if(free_==NULL){
 assert(0);
}
temp=free_;
free_=free_->next;
temp->next=head;
head=temp;
temp->hitnum=0;
return temp;
}


bool free_wp(int NO){
        WP *temp;
		WP *before;
		temp=head;
		before=head;
	   	while(temp!=NULL){
		   if(temp->NO==NO){
		     break;
		   }
		before=temp;
		temp=temp->next;
		}
        if(temp==NULL){return false;}
        if(temp->NO==head->NO){
		
		 head=head->next;
		 temp->next=free_;
		 free_=temp;
		
		}
		else if(temp->next==NULL){
		temp->next=free_;
		free_=temp;
		before->next=NULL;
		}
        else{
		  before->next=temp->next;
		  temp->next=free_;
		  free_=temp;
		}

return true;
}


bool check_point(){
WP* temp;
	  	temp=head;
 bool *success=malloc(1);
 while(temp!=NULL){
		uint32_t a;
		a=expr(temp->str,success);
    if(temp->value!=a){
	printf("Hadrware watchpoint %d:%s\n",temp->NO,temp->str);
	printf("Old Value:%d\nNew Value:%d\n\n",temp->value,a);
	temp->value=a;
	temp->hitnum++;
	return false;
	}
 temp=temp->next;
 }
 return true;



}
