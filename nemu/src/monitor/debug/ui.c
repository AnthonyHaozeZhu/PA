#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "cpu/reg.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args){
       if(args==NULL){
	     cpu_exec(1);
		 return 0;
	   }
       else{
	   int t=atoi(args);
	  	 if(t<=0){
	  	 printf("wrong parement!!\n");
	  	 return 0;
	  	 	}
	  	 else{
	   		cpu_exec(t);
			return 0;
	   		}
	   }
}

static int cmd_info(char *args){
		char s;
		if(args==NULL){
			printf("args error in cmd_info\n");
			return 0;
		
		}
		int ret=sscanf(args,"%c",&s);
		if(ret<0){
		 printf("args error\n");
		 return 0;
		}
		if(s=='r'){
				int i;

				for(i=0;i<8;i++){
				printf("%s  0x%x\n",regsl[i],reg_l(i));
				}
				printf("eip: 0x%X\n",cpu.eip);
				for(i=0;i<8;i++){
				 printf("%s  0x%x\n",regsw[i],reg_w(i));
				}
				for(i=0;i<8;i++){
					printf("%s  0x%x\n",regsb[i],reg_b(i));
				}
				return 0;
		}
		else if(s=='w'){
		  show_point();
		 	return 0;
		}
		else{
		 printf("args error!\n");
		}
		return 0;
}

static int cmd_x(char *args){
   char* N=strtok(args," ");
   if(strcmp(N,"")==0){
   printf("arg error\n");
   return 0;
   }
   char* EXPR=args+strlen(N)+1;
   bool *success=malloc(1);
   uint32_t linenumber;
	if(strcmp(EXPR,"")==0){ linenumber=0;}
   else{
    linenumber=expr(EXPR,success);
   }

	uint32_t n=atoi(N);
	uint32_t a;
	printf("Memory:");

   for(int i=0;i<n;i++){
    a=vaddr_read(linenumber,4);
     printf("\n0x%x: 0x%08x",linenumber,a);
	 linenumber+=4;
  }
   printf("\n");
	return 0;
}

static int cmd_p(char *args){
	bool *success=malloc(1);
   uint32_t res=expr(args,success);
   if(*success)
   printf("res:   %d\n",res);
   else
		printf("error\n");
	return 0;
}

static int cmd_w(char *args){
		char *EXPR=strtok(args," ");
        WP *temp;
		temp=new_wp();
		printf("%d : ",temp->NO);
        memcpy(temp->str,EXPR,strlen(EXPR));
	    printf("%s\n ",temp->str);	
		bool *success=malloc(1);
        temp->value=expr(EXPR,success);
		printf(" value:%d\n",temp->value);
		if(success){
		
			return 0;
		}
		else{
	    printf("error\n");	
		 return 0;
		}
}

static int cmd_d(char *args){
   char *N=strtok(args," ");
   int n=atoi(N);
   bool t=free_wp(n);
   if(t==false){
      printf("error:no watchpoint:%d\n",n);
   }
   else{
   		printf("success delete watchpoint:%d\n",n);
   }
   return 0;

}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","single execute",cmd_si},
  {"x","x N EXPR",cmd_x},
  {"info","information",cmd_info},
  {"w","watchpoint",cmd_w},
  {"d","delete watchpoint",cmd_d},
  {"p","p expr",cmd_p},
  /* TODO: Add more commands */

};





#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
