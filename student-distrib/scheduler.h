#ifndef _SCHEDULER_H
#define _SCHEDULER_H

// SCHEDULAR IS NOT IN USE!!!

#include "system_calls.h"
#include "filesys.h"
#include "i8259.h"
#include "paging.h"
#include "terminal.h"

#define IRQ_PIT          0

#define PIT_CMD         0x36
#define CHANNEL_0       0x40
#define CMD_REG         0x43

#define HIGH_8_BIT_MASK 0xFF00
#define LOW_8_BIT_MASK  0xFF

// initialize the pit
void pit_init(void);
// schedular that runs the scheduling
void schedule(void);

#endif
