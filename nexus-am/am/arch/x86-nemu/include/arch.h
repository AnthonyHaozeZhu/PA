#ifndef __ARCH_H__
#define __ARCH_H__

#include <am.h>

#define PMEM_SIZE (128 * 1024 * 1024)
#define PGSIZE    4096    // Bytes mapped by a page

struct _RegSet {
  // uintptr_t esi, ebx, eax, eip, edx, error_code, eflags, ecx, cs, esp, edi, ebp;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int       irq;
  uintptr_t error_code;
  uintptr_t eip;
  uintptr_t cs;
  uintptr_t eflags;
};

#define SYSCALL_ARG1(r) r -> eax
#define SYSCALL_ARG2(r) r -> ebx
#define SYSCALL_ARG3(r) r -> ecx
#define SYSCALL_ARG4(r) r -> edx


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif
