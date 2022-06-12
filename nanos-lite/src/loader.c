#include "common.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern int fs_open(const char *,int,int);
extern size_t fs_filesz(int);
extern void fs_read(int ,void *,size_t);
extern void * new_page(void );
uintptr_t loader(_Protect *as, const char *filename) {

      Log("%s",filename);
      int fd=fs_open(filename,0,0);
     size_t size=fs_filesz(fd);
//      fs_read(fd,DEFAULT_ENTRY,size);
 	void *va,*pa;
	va=DEFAULT_ENTRY;
	for(;va<size+DEFAULT_ENTRY;va+=PGSIZE){
	pa=new_page();
    Log("%x:%x",(uint32_t)pa,(uint32_t)va);
	_map(as,va,pa);
	fs_read(fd,pa,PGSIZE);
	}
		
    return (uintptr_t)DEFAULT_ENTRY;
}
