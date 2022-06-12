#include "common.h"
#include "syscall.h"


void do_SYS_none(_RegSet *r);
void do_SYS_exit(_RegSet *r);
void do_SYS_write(_RegSet *r);
void do_SYS_brk(_RegSet*r);
void do_SYS_open(_RegSet*r);
void do_SYS_read(_RegSet*r);
void do_SYS_lseek(_RegSet*r);
void do_SYS_close(_RegSet*r);


extern int fs_open(const char *path,int,int);
extern ssize_t fs_read(int,void*,size_t);
extern ssize_t fs_write(int,void*,size_t);
extern off_t fs_lseek(int,off_t,int);


_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);

  switch (a[0]) {
	case SYS_none://Log("none");
		do_SYS_none(r);break;
	case SYS_exit://Log("exit");
		do_SYS_exit(r);break;
	case SYS_write://Log("write");
		do_SYS_write(r);break;
	case SYS_brk://Log("brk");
		do_SYS_brk(r);break;
	case SYS_open://Log("open");
		do_SYS_open(r);break;
	case SYS_read://Log("read");
		do_SYS_read(r);break;
	case SYS_lseek://Log("lseek");
		do_SYS_lseek(r);break;
	case SYS_close://Log("close");
		do_SYS_close(r);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  return r;
}


void do_SYS_none(_RegSet *r){
	SYSCALL_ARG1(r)=1;
}

void do_SYS_exit(_RegSet *r){
	_halt(SYSCALL_ARG2(r));
}


extern int mm_brk(uint32_t);
void do_SYS_brk(_RegSet * r){
	 uint32_t after=(uint32_t)SYSCALL_ARG2(r);
//        Log("%x",after);
	 SYSCALL_ARG1(r)=mm_brk(after);
}



void do_SYS_close(_RegSet *r){
	SYSCALL_ARG1(r)=0;
}


void do_SYS_open(_RegSet *r){
	char* path=(char *)SYSCALL_ARG2(r);
	SYSCALL_ARG1(r)=fs_open(path,0,0);
}



void do_SYS_read(_RegSet* r){
	int fd=SYSCALL_ARG2(r);
	void *buf=(void *) SYSCALL_ARG3(r);
	size_t count=SYSCALL_ARG4(r);
//	Log("%x",buf);
	SYSCALL_ARG1(r)=fs_read(fd,buf,count);	
}

void do_SYS_lseek(_RegSet *r){
	int fd=SYSCALL_ARG2(r);
	off_t offset=SYSCALL_ARG3(r);
	int whence=SYSCALL_ARG4(r);
	SYSCALL_ARG1(r)=fs_lseek(fd,offset,whence);
}


void do_SYS_write(_RegSet *r){
	int fd=SYSCALL_ARG2(r);
	void *buf=(void*)SYSCALL_ARG3(r);
	size_t count=SYSCALL_ARG4(r);
	SYSCALL_ARG1(r)=fs_write(fd,buf,count);
}
