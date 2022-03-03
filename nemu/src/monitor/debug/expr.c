#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, 
  TK_EQ,
  TK_NUMBER,
  TK_HEX,
  TK_REG,
  TK_NEQ,
  TK_AND,
  TK_OR,
  TK_NEGATIVE,
  TK_DEREF
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0x[1-9A-Fa-f][0-9A-Fa-f]*", TK_HEX},
  {"0|[1-9][0-9]*", TK_NUMBER}, //数字
  {"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip|ax|cx|dx|bx|sp|bp|si|di|al|cl|dl|bl|ah|ch|dh|bh)", TK_REG},
  {"==", TK_EQ},
  {"!=", TK_NEQ},
  {"&&", TK_AND},
  {"\\|\\|", TK_OR},
  {"!", '!'},
  {"\\+", '+'},         // plus        // equal
  {"-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
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
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if(substr_len > 32) {
          assert(0);
        }
        if(rules[i].token_type == TK_NOTYPE) {
          break;
        }
        else {
          tokens[nr_token].type = rules[i].token_type;
          switch (rules[i].token_type) {
          case TK_NUMBER:
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            *(tokens[nr_token].str + substr_len) = '\0';
            break;
          }
          nr_token += 1;
          
          break;
        }
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}



//判断括号的匹配
bool check_parentheses(int p, int q) {
  if(p >= q) {
    //右括号少于左括号
    printf("error:p>=q in check_parntheses\n");
    return false;
  }
  // if(tokens[p].type != '(' || tokens[q].type != ')'){
  //   //括号不匹配
  //   return false;
  // }
  int cnt = 0; //记录当前未匹配的左括号的数目
  for(int curr = p + 1; curr < 1; curr++) {
    if(tokens[curr].type == '(') {
      cnt++;
    }
    if(tokens[curr].type == ')') {
      if(cnt != 0) {
        cnt--;
      }
      else {
        //左右括号不匹配
        return false;
      }
    }
  }
  if(cnt == 0) {
    return true;
  }
  else {
    return false;
  }
} 



int findDominantOp(int p, int q) {
  int i = 0, j, cnt, op = 0, pos = -1;
  for (i = p; i <= q; i++) {
    if (tokens[i].type == TK_NUMBER)
      continue;
    else if (tokens[i].type == '(') {
      cnt = 0;
      for (j = i + 1; j <= q; j++) {
        if (tokens[j].type == ')') {
          cnt++;
          i += cnt;
          break;
        }
        else {
          cnt++;
        }
      }
    }
    else {
      int opp;
      switch (tokens[i].type)
      {
        case '+':
          opp = 4;
          break;
        case '-': 
          opp = 4;
          break;
        case '*': 
          opp = 3;
          break;
        case '/':
          opp = 3;
          break;
        case TK_EQ: 
          opp = 7;
          break;
        default:
          assert(0);
          break;
      }
      if (opp >= op) {
        pos = i;
        op = opp;
      }
    }
  }
  return pos;
}

uint32_t eval(int p, int q) {
  if(p > q) {
    return -1111;
  }
  else if(p == q) {
    if (tokens[p].type == TK_NUMBER) {
      return atoi(tokens[p].str);
    }
  }
  else if(check_parentheses(p, q) == true) {
    return eval(p + 1, q - 1);
  }
  else {
    int op = findDominantOp(p, q);
    uint32_t val1 = eval(p, op - 1);
    uint32_t val2 = eval(op + 1, q);
    switch(tokens[op].type) {
      case '+':
        return val1 + val2;
      case '-': 
        return val1 - val2;
      case '/':
        return val1 / val2;
      case '*':
        return val1 * val2;
      case TK_EQ:
        return val1 == val2;
      default:
        assert(0);
    }
  }
  return 1;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();

  // return 0;
  *success = true;
  return eval(0, nr_token - 1);
}
