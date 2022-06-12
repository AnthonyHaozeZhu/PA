#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>
#include <string.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
 // Log("mul:%x::::%x",a,b);
  long long int res=(long long int)a*b;
  int *t=(int *)&res;
  int fin=((*(t)>>16)&0xffff)|(*(t+1)<<16);
 
 // Log("%x",fin);
  return (FLOAT)fin;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
 //Log("div:%x::::%x",a,b);
  int remain=0;
  int n=0;
  int res=0; 
 for(int i=0;i<32;i++){
  n=((int)a>>(31-i))&0x1;
  remain=(remain<<1)+n;
  res=res<<1;
  if(remain>(int)b){
  res=res|1;
  remain-=(int)b;
  }
 }
 for(int i=0;i<16;i++){
 n=0;
 remain=(remain<<1);
res=res<<1;
if(remain>(int)b){
res=res|1;
remain-=(int)b;
}
 
 }
 // Log("%x",res);
  return (FLOAT)res;
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
  uint32_t b=0;
   memcpy(&b,&a,sizeof(a));
 // Log("%x",b);
   FLOAT res=0;
   int32_t sign=(b>>31)&0x1;
//   printf("sign:%d\n",sign);
   int32_t e=(b>>23)&0xff;
   int32_t w=b&0x7fffff;
   int32_t l=e-127;
//   printf("l:%d\n",l);
   uint32_t mask=0x7fffff; 
   w=(w|(0x1<<23));
//   printf("w:%08x\n",w);
   if(l>0){
    w=w<<l;
   }
   else{
   w=w>>(-l);
   }
  res=(FLOAT)(w>>7); 
//  Log("%d",res);
  return sign==0?res:-res;
}

FLOAT Fabs(FLOAT a) {
  int res=(int)a;
  if(res<0){
  res=-res;
  a=(FLOAT)res;
  return a;
  }
  else{
  return a;
  }
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
