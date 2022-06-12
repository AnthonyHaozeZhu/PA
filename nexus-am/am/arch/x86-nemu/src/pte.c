#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
	
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
 for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {

		
 uint32_t t_addr=(uint32_t)pa;
 uint32_t l_addr=(uint32_t)va;		
 uint32_t pde_offset=(l_addr>>22)&0x3ff;
 uint32_t pte_offset=(l_addr>>12)&0x3ff;
 
 
 uint32_t pde_addr=(uint32_t)p->ptr;
 //
 
 uint32_t pte_addr=*((uint32_t *)(pde_addr)+pde_offset);

 if((pte_addr&0x1)==0){

	//no pte,only pde
 pte_addr=(uint32_t)(palloc_f());
 for(int i=0;i<NR_PTE;i++){
 *((uint32_t *)(pte_addr)+i)=0;
 }

 *((uint32_t*)(pde_addr)+pde_offset)=pte_addr|0x1;
 *((uint32_t*)(pte_addr)+pte_offset)=t_addr|0x1;
 return;
 }

 *((uint32_t*)(pte_addr&0xfffff000)+pte_offset)=t_addr|0x1; 
 

}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {

	*((uint32_t*)(ustack.end)-1)=(uint32_t)0;
	*((uint32_t*)(ustack.end)-2)=(uint32_t)0;
    //eflag
    *((uint32_t*)(ustack.end)-4)=((0x00000002)|(1<<9));
    //cs
	*((uint32_t*)(ustack.end)-5)=0x8;
	//eip
	*((uint32_t*)(ustack.end)-6)=(uint32_t)entry;

  return (_RegSet*)((uint32_t*)(ustack.end)-16);
}
