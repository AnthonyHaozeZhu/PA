#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  return inl(RTC_PORT)-boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  int i;
  int j;
  for (i =y ; i<y+h ; i++) {
		  for(j=x;j<x+w;j++){
		  	fb[j+i*_screen.width]=pixels[(j-x)+(i-y)*w];
		  }
  }
}

void _draw_sync() {
}

int _read_key() {
   uint32_t KEY_CODE=_KEY_NONE;	
   if(inb(0x64)&0x1){
     KEY_CODE=inl(0x60);
   }
  return KEY_CODE;
}
