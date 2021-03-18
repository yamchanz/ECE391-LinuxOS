#ifndef _IDT_HANDLERS_H
#define _IDT_HANDLERS_H

#include "x86_desc.h"

#define EX_ARR_SIZE 20
#define EXCEPTION_15 14

// initialize IDT at bootup
extern void initialize_idt();

#endif /* _IDT_HANDLERS_H */