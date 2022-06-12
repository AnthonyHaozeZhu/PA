#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128*1024*1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];
paddr_t page_translate(vaddr_t addr,int type);

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
 int a=is_mmio(addr);
 if(a==-1){
  return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
 }
 else{
 	return mmio_read(addr,len,a);
 }
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
	int NO=is_mmio(addr);
	if(NO==-1){
  memcpy(guest_to_host(addr), &data, len);
	}
	else{
	mmio_write(addr,len,data,NO);
	}
}

uint32_t vaddr_read(vaddr_t addr, int len) {
	if(cpu.CR0.PG==0){
	  return paddr_read(addr,len);
	}

  if(((addr&0xfffff000)^((addr+len-1)&0xfffff000))!=0){
//	Log("%x",addr);
	paddr_t n=(addr+len)&(0xfffff000);
	int l=n-addr;
//	Log("len:%d",len);
//	Log("%x",paddr_read(addr,len));
//	Log("%x",(paddr_read(page_translate(addr,0),l))| (paddr_read(page_translate(n,0),len-l)<<(8*l)));
	return (paddr_read(page_translate(addr,0),l))| (paddr_read(page_translate(n,0),len-l)<<(8*l));
  }
 else{
  paddr_t paddr=page_translate(addr,0);
  return paddr_read(paddr, len);
 }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
if(cpu.CR0.PG==0){

paddr_write(addr,len,data);
return;
}

if(((addr&0xfffff000)^((addr+len-1)&0xfffff000))!=0x0){
  	//	Log("%x",addr);
	paddr_t n=(addr+len)&(0xfffff000);
	int l=n-addr;	
	paddr_write(page_translate(addr,1),l,data);
	paddr_write(page_translate(n,1),len-l,data>>(8*l));
  }
 else{
  paddr_t paddr=page_translate(addr,1);
  paddr_write(paddr,len,data);
 }
}

paddr_t page_translate(vaddr_t addr,int type){
	if(cpu.CR0.PG!=0x1){
		return addr;
	}
//	Log("%x",addr);	
		int offset=addr&0xfff;
	uint32_t pde=(cpu.CR3.val&0xfffff000);
//	Log("%x",cpu.CR3.PAGE_BASE);
//	Log("%x",cpu.CR3.val);
	int d=((addr>>22)&0x3ff);
	pde+=(4*d);
//    Log("d:%d",d);

	uint32_t pte_addr=paddr_read(pde,4);

	//a
	paddr_write(pde,4,(pte_addr|0x20));
	assert((pte_addr&0x1)==0x1);
	
	pte_addr&=0xfffff000;
	int t=((addr>>12)&0x3ff);

	pte_addr+=4*t;

	uint32_t page_addr=paddr_read(pte_addr,4);
	//a,d
	paddr_write(pte_addr,4,((page_addr|0x20)|(type<<6)));
//	Log("%x",page_addr);
	if((page_addr&0x1)==0){
	Log("addr:%x",addr);
	Log("pte:%x",4*t);
	Log("%x",pte_addr);
	Log("%x",page_addr);
//	for(int i=-10;i<10;i++){
  //       page_addr=paddr_read(pte_addr+4*i,4);
//		 Log("a:%x",page_addr);
//	}
	assert(0);
	}	
	//assert((page_addr&0x1)==0x1);
	uint32_t true_addr=((page_addr&0xfffff000)|offset);


	return true_addr;
}
