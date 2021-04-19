#ifndef _IDT_HANDLERS_H
#define _IDT_HANDLERS_H

#include "x86_desc.h"
#include "system_calls.h"

#define EX_ARR_SIZE 20
#define SYS_CALL_IDX 0x80
#define RTC_IDX 0x28
#define KEYBOARD_IDX 0x21

// initialize IDT at bootup
extern void initialize_idt();

// install handler into IDT at a specific index
extern void install_interrupt_handler(int idt_offset, void (*handler), int trap, int sys_call);
extern void install_trap_handler(int idt_offset, void (*handler));


#endif /* _IDT_HANDLERS_H */
