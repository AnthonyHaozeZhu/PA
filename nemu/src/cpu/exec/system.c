#include "cpu/exec.h"
void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  //id_dest addri
  //
  
//	for(int i=0;i<10;i++){
//		rtl_lm(&t0,&id_dest->addr,4);
//		Log("%08x",t0);
//		rtl_addi(&id_dest->addr,&id_dest->addr,4);
//	}
    rtl_lm(&t0,&id_dest->addr,2);
	cpu.IDTR.limit=t0;
	rtl_addi(&id_dest->addr,&id_dest->addr,2);
	rtl_lm(&t1,&id_dest->addr,4);
	cpu.IDTR.base=t1;
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
//	Log("%d",id_dest->reg); 
  switch(id_dest->reg){
  	case 0:
		cpu.CR0.val=id_src->val;
		break;
	case 3:
		cpu.CR3.val=id_src->val;
		break;	
  	default:Log("gg");assert(0);
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
 
switch(id_dest->reg){
		case 0:
		t0=cpu.CR0.val;
		rtl_sr(id_src->reg,4,&t0);
		break;
		case 2:
		t0=cpu.CR3.val;
		rtl_sr(id_src->reg,4,&t0);
		break;
		default:Log("gg");assert(0);
}

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

extern void raise_intr(uint8_t NO,vaddr_t ret_addr);

make_EHelper(int) { 
  raise_intr(id_dest->val,*eip);
 // decoding.is_jmp=1;
 // decoding.jmp_eip=t3;
//  Log("int:%x",t3);
  print_asm("int %s", id_dest->str);
#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  rtl_pop(&t2);
  decoding.is_jmp=1;
  decoding.jmp_eip=t2;
// Log("iret:%x",t2);
  rtl_pop(&t2);
  cpu.CS=t2;
  rtl_pop(&t2);
  //Log("%x",t2);
  cpu.EFLAGS.val=t2;
  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  
  t0=pio_read(id_src->val,id_dest->width);
  operand_write(id_dest,&t0);
  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  pio_write(id_dest->val,id_src->width,id_src->val);

  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
