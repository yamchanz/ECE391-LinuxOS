#ifndef _RTC_H
#define _RTC_H

#include "types.h"

#define IRQ_RTC 8
#define SELECT_REG  0x70
#define DATA_REG    0x71
#define NMI_REG_A   0x8A
#define NMI_REG_B   0x8B
#define REG_C       0x0C

void initialize_rtc();

#endif
