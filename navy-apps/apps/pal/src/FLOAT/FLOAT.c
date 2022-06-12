#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>
#include <string.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
 // Log("mul:%x::::%x",a,b);
  return (a * b) >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
 //Log("div:%x::::%x",a,b);
  FLOAT x = Fabs(a);
  FLOAT y = Fabs(b);
  FLOAT ret = x / y;
  x = x % y;

  for (int i = 0; i < 16; i++) {
    x <<= 1;
    ret <<= 1;
    if (x >= y) {
      x -= y;
      ret++;
    }
  }
  if (((a ^ b) & 0x80000000) == 0x80000000) {
    ret = -ret;
  }
  return ret;
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
  union float_ {
    struct {
      uint32_t M : 23;//有效位
      uint32_t E : 8;//指数位
      uint32_t S : 1;//符号位
    };
    uint32_t val;
  };
  union float_ f;
  f.val = *((uint32_t*)(void*)&a);

  //将二进制浮点数转换为真的浮点数
  int exp = f.E - 127;//真实指数需减去固定偏移值(移码)

  FLOAT ret;
  //FLOAT 最后16位为小数，又有效位为23,
  //所以当指数大于７时，小数位不足，需要左移；
  //当指数小于７，小数溢出，需要右移
  if (exp <= 7) {
    ret = (f.M | (1 << 23)) >> 7 - exp;
  }
  else {
    ret = (f.M | (1 << 23)) << (exp-7);
  }
  return f.S == 0 ? ret : (ret|(1<<31));
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
