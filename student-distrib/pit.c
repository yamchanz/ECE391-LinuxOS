#include "pit.h"

volatile int schedule_init;

// reference http://www.jamesmolloy.co.uk/tutorial_html/9.-Multitasking.html
void schedule() {
    // first bootup - start the three terminals
    if(schedule_init) {
        int i;
        for(i = 0; i < 3; i++) {
            // if term was not booted
            if(term[i].pcb == NULL) {

                // clone directory

                // clone page table
                execute((uint8_t*)"shell");
            }
        }
        schedule_init = 0;
    }

    // second bootup


    // switch ESP/EBP to next process' kernel stack
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8_MB - _8_KB * pcb->pid - FOUR_BYTE;

    // restore next process' TSS

    // flush TLB on process switch


    return;
}

// reference PIT initialization from http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html ,
// https://wiki.osdev.org/Programmable_Interval_Timer
void pit_init () {
    // initialize PIT to interrupts at 10ms frequency (10ms = 100Hz)
    schedule_init = 1;

    // disable interrupts to set registers
    cli();

    uint32_t = freq = 100;
    uint32_t divisor = 1193180 / freq;

    // set command register - select channel 0, lobyte/hibyte, and rate generator - 0011 0110 
    outb(PIT_CMD, CMD_REG);

    // set high and low byte using outb (1 byte at a time)
    // outb(data, port)
    outb(freq & LOW_8_BIT_MASK, CHANNEL_0);
    outb((freq & HIGH_8_BIT_MASK) >> 8, CHANNEL_0);

    sti();

    enable_irq(IRQ_PIT);

    return;
}