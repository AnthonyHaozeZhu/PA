#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  // long long int result =  (long long int) a * b;
  // int *temp = (int *)& result;
  // return (FLOAT)((*(temp)>>16)& 0xffff) | (*(temp + 1) << 16);
  
  assert(0);
  return 0;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {

  // FLOAT x = Fabs(a);
  // FLOAT y = Fabs(b);
  // FLOAT result = x / y;
  // x = x % y;
  // for (int i = 0; i < 16; i++) {
  //   x <<= 1;
  //   result <<= 1;
  //   if (x >= y) {
  //     x -= y;
  //     result++;
  //   }
  // }
  // if (((a ^ b) & 0x80000000) == 0x80000000) {
  //   result = -result; 
  // }
  // return result;
  assert(0);
  return 0;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */
  // Log("This is in it!")
  // union float_ {
  //   struct {
  //     uint32_t m: 23; //浮点数表示的有效位
  //     uint32_t e: 8;  //浮点数表示的指数位
  //     uint32_t s: 1;  //浮点数表示的符号位
  //     /* data */
  //   };
  //   uint32_t value;
  // };
  // uint32_t temp = 0;
  // memcpy(&temp, &a, sizeof(a));
  // union float_ f;
  // f.value = temp;
  // int e = f.e - 127;

  // FLOAT result;

  // if (e <= 7) {
  //   result = (f.m | (1 << 23)) >> 7 - e;//1<<23代表缺省的1 
  // }
  // else {
  //   result = (f.m | (1 << 23)) << (e - 7);
  // }
  // if(f.s == 0) {
  //   return result;
  // }
  // else {
  //   return (result | (1<<31));
  // }
    assert(0);
  return 0;
}

FLOAT Fabs(FLOAT a) {
  assert(0);
  return 0;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}
