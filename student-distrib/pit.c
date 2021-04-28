#include "pit.h"

volatile int schedule_init;

/* schedule - CP5
 * reference http://www.jamesmolloy.co.uk/tutorial_html/9.-Multitasking.html
 * parameters - none
 * returns - none
 */
void schedule() {
    // first bootup - start the three terminals
    if(schedule_init) {
        int i;
        for(i = 0; i < 3; i++) {
            // for pid 0 to 2,  assign to each terminal and init
            // t[i].pid_[0] = i;

            // clone directory

            // clone page table
            execute((uint8_t*)"shell");
        }
        schedule_init = 0;

        return;
    }

    // second bootup

    // get next process pcb

    // switch ESP/EBP to next process' kernel stack
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8_MB - _8_KB; // * (/*whichever pid we are on the terminal we are on*/) - 4;

    // restore next process' TSS
    // asm volatile(
    //     "movl %%esp, %%eax"
    //     "movl %%bp, %%ebx"
    //     :"=a"(old_pcb->esp), "=b"(old_pcb->ebp) // output
    //     : // input
    // );
    // asm volatile(
    //     "movl %%ebx, %%esp"
    //     "movl %%ecx, %%ebp"
    //     : //ouput
    //     :"b"(next_pcb->esp), "c"(next_pcb->ebp) //input
    // );
    // flush TLB on process switch


    return;
}

/* pit_init - CP5
 * Initializes PIT (programmable interrupt controller) for scheduling.
 * Frequency to be set is from range 20Hz to 100Hz.
 * (frequency = 1193182 / reload_value Hz)
 * reference PIT initialization from http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html ,
 *                                   https://wiki.osdev.org/Programmable_Interval_Timer
 * parameters - none
 * returns - none
 */
void pit_init () {
    // initialize PIT to interrupts at 10ms frequency (10ms = 100Hz)
    schedule_init = 1;
    uint32_t freq = 100;
    // uint32_t divisor = 1193182 / freq;
    // disable interrupts to set registers
    cli();
    // set command register - select channel 0, lobyte/hibyte, and rate generator - 0011 0110 
    outb(PIT_CMD, CMD_REG);
    // set high and low byte using outb (1 byte at a time)
    // outb(data, port)
    outb(freq & LOW_8_BIT_MASK, CHANNEL_0);
    outb((freq & HIGH_8_BIT_MASK) >> 8, CHANNEL_0);
    sti();
    // PIT is connected directly to IRQ0
    enable_irq(IRQ_PIT);
}
