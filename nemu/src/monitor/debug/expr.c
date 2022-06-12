#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include<string.h>
enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUMBER,L,R,TK_HEX,TK_CPU,DEREF,
   OR,EQ,NEQ,AND,NEGATIVE,
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
 {"!=",NEQ},
  {"==",EQ},
  {"\\|\\|",OR},
  {"&&",AND},
  {"!",'!'},
  {"\\$[A-Z a-z]+",TK_CPU},
  {"0x[0-9 a-f A-F]+",TK_HEX},
  {"\\(",L},
  {"\\)",R},
  {"[0-9]+",TK_NUMBER},
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'}, 		 // plus
  {"==", TK_EQ},         // equal
  {"\\-",'-'},
  {"\\*",'*'},
  {"\\/",'/'}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;
  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
       // char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
          //  i, rules[i].regex, position, substr_len, substr_len, substr_start);
       	if(rules[i].token_type==TK_NOTYPE){
				position+=substr_len;break;
		}


		tokens[nr_token].type=rules[i].token_type;
        if(rules[i].token_type==TK_HEX){
				memcpy(tokens[nr_token++].str,e+position+2,substr_len-2);
				tokens[nr_token-1].str[substr_len-2]='\0';
				position+=substr_len;break;
		}
		memcpy(tokens[nr_token++].str,e+position,substr_len);
		position +=substr_len;
		tokens[nr_token-1].str[substr_len]='\0';
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p,int q){
	if(tokens[p].type!=L||tokens[q].type!=R){
			return false;}

	int count=0;
	for(int i=p+1;i<q-1;i++){
			if(tokens[i].type==L){
			count++;
			}
			else if(tokens[i].type==R){
					count--;}
			if(count<0){return false;}
	}

	return true;

}

int findOp(int p,int q){
 //&&||
  //==,!=
   //+,-
  //*/
  //=,!,*
	    int a[5]={-1,-1,-1,-1,-1};
		int f;
		for(int i=p;i<=q;i++){
                if(tokens[i].type==L){
						f=i;
				    while(check_parentheses(f,i)!=true){
					i++;
					}
			
				}
				else if(tokens[i].type==AND||tokens[i].type==OR){
				   a[0]=i;
				}
				else if(tokens[i].type==EQ||tokens[i].type==NEQ){
					a[1]=i;
				}
				else if(tokens[i].type=='+'||tokens[i].type=='-'){
						a[2]=i;
				}
				else if(tokens[i].type=='*'||tokens[i].type=='/'){
						a[3]=i;
				}
				else if(tokens[i].type==DEREF||tokens[i].type==NEGATIVE||tokens[i].type=='!'){
				   if(a[4]==-1)
				        a[4]=i;
				}
		}

       for(int i=0;i<5;i++){
	   if(a[i]!=-1){return a[i];}
	   }
	   printf("something go wrong!\n");
	   assert(0);
	   return 0;

}



int eval(int p,int q){
		
	if(p>q){
		return 0;
	}
	else if(p==q)
	{
			if(tokens[p].type==TK_HEX){
				uint32_t a;
				printf("%s\n",tokens[p].str);
				sscanf(tokens[p].str,"%x",&a);
				return a;
			}
			else if(tokens[p].type==TK_CPU)
			{    
				if(strcmp(tokens[p].str+1,"eip")==0){
				  return cpu.eip;
				}

                  for(int i=0;i<8;i++){
				  if(strcmp(tokens[p].str+1,regsl[i])==0){
				      return reg_l(i);
				  }
                  else if(strcmp(tokens[p].str+1,regsw[i])==0){
				  	return reg_w(i);
				  }
				  else if (strcmp(tokens[p].str+1,regsb[i])==0){
				   return reg_b(i);
				  }
				  
				  }
				  return 0;
			}

		return atoi(tokens[p].str);	 
	}
	else if(check_parentheses(p,q)==true)
	{
	    return eval(p+1,q-1);
	}
	else{
	   
	int s=findOp(p,q);		
	
		if(tokens[s].type=='+'){
				return eval(p,s-1)+eval(s+1,q);
	
		}
		else if(tokens[s].type=='-'){
			return eval(p,s-1)-eval(s+1,q);
			
		}

	   else	if(tokens[s].type=='*'){
		  return eval(p,s-1)*eval(s+1,q);
		}
		else if(tokens[s].type=='/'){
		
	     return eval(p,s-1)/eval(s+1,q);	
		
		}
	
	else if(tokens[s].type==OR){return eval(p,s-1)||eval(s+1,q);}
	else if(tokens[s].type==AND){return eval(p,s-1)&&eval(s+1,q);}
	else if(tokens[s].type==EQ){return eval(p,s-1)==eval(s+1,q);}
	else if(tokens[s].type==NEQ){return eval(p,s-1)!=eval(s+1,q);}
    else if(tokens[s].type==NEGATIVE&&s==p){return -eval(p+1,q);}
	else if(tokens[s].type==DEREF&&s==p){return vaddr_read(eval(p+1,q),4);}
 	else if(tokens[s].type=='!'&&s==p){return !(eval(p+1,q));}
	printf("wrong expresssion!\n");
	return 0;
	}

}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
for(int i=0;i<nr_token;i++){
		if(tokens[i].type=='*'&&(i==0||(tokens[i-1].type!=TK_NUMBER&&tokens[i-1].type!=TK_CPU&&tokens[i-1].type!=TK_HEX))){
						tokens[i].type=DEREF;
	}
		else if(tokens[i].type=='-'&&(i==0||(tokens[i-1].type!=TK_NUMBER&&tokens[i-1].type!=TK_CPU&&tokens[i-1].type!=TK_HEX))){
						tokens[i].type=NEGATIVE;
						
	}

	}

  uint32_t res=eval(0,nr_token-1);
*success=1;
  return res;
}
