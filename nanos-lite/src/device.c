#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

extern void change();
size_t events_read(void *buf, size_t len) {
  int keycode=_read_key();
  if(keycode!=0){
	if(keycode&0x8000){
	    sprintf(buf,"kd %s\n",keyname[keycode&0x00ff]);
		//Log("%x",keycode);
		if(keycode==0x800d){
			change();
		}
	}
        else{
 	    sprintf(buf,"ku %s\n",keyname[keycode]);
	}
	return strlen(buf);
  }
  
  sprintf(buf,"t %d\n",_uptime());
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  memcpy(buf,dispinfo+offset,len);
}

extern _Screen _screen;
void fb_write(const void *buf, off_t offset, size_t len) {
 if(len<=0){return;}
  offset=offset/4;
  int x=offset%_screen.width;
  int y=offset/_screen.width;
   _draw_rect((uint32_t*)buf,x,y,len/4,1);

}

void init_device() {
  _ioe_init();
  sprintf(dispinfo,"WIDTH:%d\n HEIGHT:%d\n",_screen.width,_screen.height);
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
