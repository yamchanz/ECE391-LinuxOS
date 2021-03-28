#ifndef _RTC_H
#define _RTC_H

#include "types.h"

#define IRQ_RTC 8
#define SELECT_REG  0x70
#define DATA_REG    0x71
#define NMI_REG_A   0x8A
#define NMI_REG_B   0x8B
#define REG_C       0x0C
#define TWO_HZ_FREQ 0x0F
#define BIT_6_MASK  0x40
#define HIGH_BIT_MASK 0xF0
#define INT_BYTES   4
#define HIGH_LIMIT_FREQ 1024
#define NUM_READ_INTS  20

volatile int rtc_int_received;

void initialize_rtc();

int32_t rtc_open(const uint8_t* filename);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_close(int32_t fd);



#endif
