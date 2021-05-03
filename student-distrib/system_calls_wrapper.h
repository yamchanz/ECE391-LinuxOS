#ifndef _SYSTEM_CALLS_WRAPPER_H
#define _SYSTEM_CALLS_WRAPPER_H

// assembly linkage for system calls interrupt handler
extern void context_switch(uint32_t entry_point);
// 
extern void halt_ret(uint32_t esp, uint32_t ebp, uint32_t status);

#endif
