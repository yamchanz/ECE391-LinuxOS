#ifndef _CONTEXT_SWITCH_H
#define _CONTEXT_SWITCH_H
#include "types.h"

extern void context_switch(uint32_t esp, uint32_t entry_point);

#endif
