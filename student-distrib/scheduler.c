#include "scheduler.h"

volatile int schedule_init;

void pit_init(void) {
    // initialize PIT to interrupts at 10ms frequency (10ms = 100Hz)
    schedule_init = 1;
    uint32_t freq = 100;
    uint32_t divisor = 1193182 / freq;
    // disable interrupts to set registers
    cli();
    // set command register - select channel 0, lobyte/hibyte, and rate generator - 0011 0110
    outb(PIT_CMD, CMD_REG);
    // set high and low byte using outb (1 byte at a time)
    outb(divisor & LOW_8_BIT_MASK, CHANNEL_0);
    outb((divisor & HIGH_8_BIT_MASK) >> 8, CHANNEL_0);
    sti();
    enable_irq(IRQ_PIT);
}

void schedule(void) {
    // first bootup - start the three terminals
    if(schedule_init) {
        int i;
        for(i = 0; i < 3; i++) {
                // clone directory
                // clone page table
                // execute((uint8_t*)"shell");
        }
        schedule_init = 0;
    }
    // second bootup
    // printf("a");
    // switch ESP/EBP to next process' kernel stack
    //tss.ss0 = KERNEL_DS;
    //tss.esp0 = _8_MB - _8_KB * pcb->pid - FOUR_BYTE;
    // restore next process' TSS
    // flush TLB on process switch
}
