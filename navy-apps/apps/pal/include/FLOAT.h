#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "assert.h"

typedef int FLOAT;

static inline int F2int(FLOAT a) {
  if((a & 0x80000000) ==0) {
    return a >> 16;
  }
  else {
    return -((-a) >> 16);
  }
  // assert(0);
  // return 0;
}

static inline FLOAT int2F(int a) {
  int b;
  if(a < 0) {
    b = -a;
  }
  else {
    b = a;
  }
  FLOAT result = (b << 16);
  if(a < 0) {
    result = - result;
  }
  return result;
  // assert(0);
  // return 0;
}

FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT Fsqrt(FLOAT);
FLOAT Fpow(FLOAT, FLOAT);

static inline FLOAT F_mul_int(FLOAT a, int b) {
  b = int2F(b);
  FLOAT result = F_mul_F(a, b);
  return result;
  // assert(0);
  // return 0;
}

static inline FLOAT F_div_int(FLOAT a, int b) {
  b = int2F(b);
  FLOAT result = F_div_F(a, b);
  return result;
  // assert(0);
  // return 0;
}

#endif
