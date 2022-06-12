#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>
#include <string.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
 // Log("mul:%x::::%x",a,b);
  return (a * b) >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  FLOAT result = Fabs(a) / Fabs(b);
  FLOAT m = Fabs(a);
  FLOAT n = Fabs(b);
  m = m % n;

  for (int i = 0; i < 16; i++) {
    m <<= 1;
    result <<= 1;
    if (m >= n) {
      m -= n;
      result++;
    }
  }
  if (((a ^ b) & 0x80000000) == 0x80000000) {
    result = -result;
  }
  return result;
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
      uint32_t m : 23;
      uint32_t e : 8;
      uint32_t signal : 1;
    };
    uint32_t value;
  };
  union float_ f;
  f.value = *((uint32_t*)(void*)&a);

  int e = f.e - 127;

  FLOAT result;
  if (e <= 7) {
    result = (f.m | (1 << 23)) >> 7 - e;
  }
  else {
    result = (f.m | (1 << 23)) << (e - 7);
  }
  return f.signal == 0 ? result : (result|(1<<31));
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
