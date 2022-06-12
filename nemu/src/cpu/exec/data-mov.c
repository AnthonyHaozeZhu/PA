#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
if(id_dest->width==1){
	rtl_sext(&id_dest->val,&id_dest->val,1);
}
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_dest->val);
  operand_write(id_dest,&id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
//	Log("%s","pusha!!!");
  rtl_lr(&t1,4,4);
  rtl_lr(&t2,0,4);
  rtl_push(&t2);
  rtl_lr(&t2,1,4);
  rtl_push(&t2);
  rtl_lr(&t2,2,4);
  rtl_push(&t2);
  rtl_lr(&t2,3,4);
  rtl_push(&t2);
  rtl_push(&t1);
  rtl_lr(&t2,5,4);
  rtl_push(&t2);
  rtl_lr(&t2,6,4);
  rtl_push(&t2);
  rtl_lr(&t2,7,4);
  rtl_push(&t2);
  print_asm("pusha");
}

make_EHelper(popa) {
//	Log("popa!");
  rtl_pop(&t2);
  rtl_sr(7,4,&t2);
   rtl_pop(&t2);
  rtl_sr(6,4,&t2);
   rtl_pop(&t2);
  rtl_sr(5,4,&t2);
    rtl_pop(&t2);
	rtl_pop(&t2);
  rtl_sr(3,4,&t2);
   rtl_pop(&t2);
  rtl_sr(2,4,&t2);
   rtl_pop(&t2);
  rtl_sr(1,4,&t2);
  rtl_pop(&t2);
  rtl_sr(0,4,&t2);
 print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&t2);
  cpu.ebp=t2;
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    rtl_lr(&t0,0,2);
	rtl_sext(&t0,&t0,2);
	rtl_sari(&t0,&t0,31);
	rtl_sr(2,2,&t0);
  }
  else {
    rtl_sari(&cpu.edx,&cpu.eax,31);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    rtl_lr(&t0,0,1);
	rtl_sext(&t2,&t0,1);
	rtl_sr(0,2,&t0);
  }
  else {
    rtl_lr(&t0,0,2);
	rtl_sext(&t2,&t0,2);
	rtl_sr(0,4,&t0);
   }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
