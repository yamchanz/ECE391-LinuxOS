#include "scheduler.h"

volatile int schedule_init;

void pit_init(void) {
    // initialize PIT to interrupts at 10ms frequency (10ms = 100Hz)
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
    // round robin scheduling
    cli();
    int i;
    int32_t t_next = (t_cur + 1) % TERMINAL_COUNT;
    
    // first three bootup - start the three terminals
    if(t[t_cur].shell_flag == -1 /* maybe use -1 for nothing */) {

        // process_status[t_cur] = 1; /* in the future set to terminal id  */
        t[t_cur].running_process = t_cur;
        // t[t_cur].shell_flag = 0;
        // parent pid of base is parent
        // parent_pid_arr[i] = i;
        // put current esp and ebp into pcb
        pcb_t* pcb = get_pcb(t_cur);

        asm volatile(
            "movl %%esp, %0     \n"
            "movl %%ebp, %1     \n"
            :"=r"(pcb->cur_esp), "=r"(pcb->cur_ebp) // output
            : // input
        );
        ++i;
        
        execute((uint8_t*)"shell");

        // we will never get here
        return;
    }

    // all other schedule calls
    if(t[t_next].shell_flag == -1 || i == 3) {
        t_cur = t_next;
        send_eoi(IRQ_PIT);
        if (i == 3) 
            i = 0;
        return;
    }
    // get next process pcb
    pcb_t* old_pcb = get_pcb(t[t_cur].running_process);
    pcb_t* cur_pcb = get_pcb(t[t_next].running_process);

    // switch ESP/EBP to next process' kernel stack
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8_MB - _8_KB * cur_pcb->pid - 4;

    // remap + flush TLB 
    map_program(cur_pcb->pid);
    
    
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

    t_cur = t_next;
    sti();
    return;

}
