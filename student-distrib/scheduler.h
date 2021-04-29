#ifndef _SCHEDULER_H
#define _SCHEDULER_H

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

void pit_init(void);
void schedule(void);

#endif
