#include "scheduler.h"

volatile int schedule_init;
volatile uint8_t cur_ter = 0;
uint8_t next_ter = 0;

void schedule(void) {
    next_ter = (cur_ter + 1) % TERMINAL_COUNT;

    map_program(term[next_ter].cur_pid_idx);

    pcb_t* old_pcb = get_pcb(term[cur_ter].cur_pid_idx);
    pcb_t* cur_pcb = get_pcb(term[next_ter].cur_pid_idx);
    
   
    // switch ESP/EBP to next process' kernel stack
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8_MB - _8_KB * term[next_ter].cur_pid_idx - 4;
    
    cur_ter = next_ter;

    // restore next process' TSS
    asm volatile(
        "movl %%esp, %0\n\
        movl %%ebp, %1"
        :"=r"(old_pcb->cur_esp), "=r"(old_pcb->cur_ebp) // output
        : // input
    );

    // process switch
    asm volatile(
        "movl %0, %%esp\n\
        movl %1, %%ebp"
        : //ouput
        :"r"(cur_pcb->cur_esp), "r"(cur_pcb->cur_ebp) //input
    );

    return;

}

void PIT_handler(){
    send_eoi(IRQ_PIT);

    cli();
    if(term[1].t_status = 1 || term[2].t_status = 1){
        schedule();
    }
}

void pit_init(void) {
    // initialize PIT to interrupts at 10ms frequency (10ms = 100Hz)
    schedule_init = 1;
    // disable interrupts to set registers
    // cli();
    uint32_t freq = 100;
    uint32_t divisor = 1193182 / freq;
    // set command register - select channel 0, lobyte/hibyte, and rate generator - 0011 0110
    outb(PIT_CMD, CMD_REG);
    // set high and low byte using outb (1 byte at a time)
    outb(divisor & LOW_8_BIT_MASK, CHANNEL_0);
    outb((divisor & HIGH_8_BIT_MASK) >> 8, CHANNEL_0);

    enable_irq(IRQ_PIT);
}
