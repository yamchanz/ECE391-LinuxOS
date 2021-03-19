#ifndef _IDT_HANDLERS_H
#define _IDT_HANDLERS_H

#include "x86_desc.h"

#define EX_ARR_SIZE 20
#define EXCEPTION_15 14

// initialize IDT at bootup
extern void initialize_idt();

// install handler into IDT at a specific index
extern void install_task_handler(int idt_offset, void (*handler));
extern void install_interrupt_handler(int idt_offset, void (*handler));
extern void install_trap_handler(int idt_offset, void (*handler));


#endif /* _IDT_HANDLERS_H */
