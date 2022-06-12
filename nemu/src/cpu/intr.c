#include "cpu/exec.h"


void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
	//Log("%x",cpu.EFLAGS.val);
	t0=cpu.EFLAGS.val;
	rtl_push(&t0);
	cpu.EFLAGS.IF=0;
	t0=cpu.CS;
	rtl_push(&t0);
	t0=ret_addr;
	rtl_push(&t0);
  	t2=cpu.IDTR.base;
	t1=cpu.IDTR.limit;
    //cpu.EFLAGS.IF=0; 
	//Log("%x",cpu.CS);
	//Log("ret_addr::%x",ret_addr);
	//Log("NO:%x",NO);
	assert(NO*8-1<=t1);
	rtl_li(&t3,4*NO);
	rtl_add(&t2,&t2,&t3);
	rtl_add(&t2,&t2,&t3);
	rtl_lm(&t0,&t2,4);
	rtl_addi(&t2,&t2,4);
   	rtl_lm(&t1,&t2,4);
  	//Log("%x",(int)t0);	
	//Log("%x",(int)t1);
	assert((t1&0x00008000)==0x00008000);
	rtl_andi(&t0,&t0,0xffff);
	rtl_andi(&t1,&t1,0xffff0000);
	rtl_or(&t3,&t0,&t1);
	
	decoding.is_jmp=1;
	decoding.jmp_eip=t3;
	//t3
}

void dev_raise_intr() {
		cpu.INTR=1;
}
