#ifndef _PIT_H
#define _PIT_H

#include "system_calls.h"
#include "filesys.h"
#include "i8259.h"
#include "paging.h"
#include "terminal.h"

/*
PIT Mode/Command register

Bits         Usage
6 and 7      Select channel :
                0 0 = Channel 0
                0 1 = Channel 1
                1 0 = Channel 2
                1 1 = Read-back command (8254 only)
4 and 5      Access mode :
                0 0 = Latch count value command
                0 1 = Access mode: lobyte only
                1 0 = Access mode: hibyte only
                1 1 = Access mode: lobyte/hibyte
1 to 3       Operating mode :
                0 0 0 = Mode 0 (interrupt on terminal count)
                0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                0 1 0 = Mode 2 (rate generator)
                0 1 1 = Mode 3 (square wave generator)
                1 0 0 = Mode 4 (software triggered strobe)
                1 0 1 = Mode 5 (hardware triggered strobe)
                1 1 0 = Mode 2 (rate generator, same as 010b)
                1 1 1 = Mode 3 (square wave generator, same as 011b)
0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
 */

#define IRQ_PIT                 0

#define CHANNEL_0               0x40
#define CMD_REG                 0x43
#define PIT_CMD                 0x36

#define HIGH_8_BIT_MASK         0xFF00
#define LOW_8_BIT_MASK          0xFF

int shell_init;

int32_t t_next;
int32_t t_cur;

/* initializes PIT */
extern void pit_init(void);
/* scheduler for PIT interrupt */
extern void schedule(void);


#endif
