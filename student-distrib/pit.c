#include "pit.h"

int32_t t_next;
/* schedule - CP5
 * reference http://www.jamesmolloy.co.uk/tutorial_html/9.-Multitasking.html
 * parameters - none
 * returns - none
 */
static int i = 0;

void schedule() {
    t_next = (t_run + 1) % TERMINAL_COUNT;
    // first bootup - start the three terminals
    if(t[t_run].pid_[0] == -1 /* maybe use -1 for nothing */) {

        shell_init = 1;
        // for pid 0 to 2,  assign to each terminal and init
        t[t_run].pid_[0] = i++;
        t[t_run].cur_pid_idx = 0;
        
        // parent pid of base is parent
        // parent_pid_arr[i] = i;
        // put current esp and ebp into pcb
        pcb_t* pcb = get_pcb(t_run);

        asm volatile(
            "movl %%esp, %0     \n"
            "movl %%ebp, %1     \n"
            :"=r"(pcb->cur_esp), "=r"(pcb->cur_ebp) // output
            : // input
        );
        t_run = t_next;
        execute((uint8_t*)"shell");

        // we will never get here
        return;
    }

    // all other schedule calls
    
    // get next process pcb
    pcb_t* old_pcb = get_pcb(t[t_run].pid_[t[t_run].cur_pid_idx]);
    pcb_t* cur_pcb = get_pcb(t[t_next].pid_[t[t_next].cur_pid_idx]);
    // switch ESP/EBP to next process' kernel stack
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8_MB - _8_KB * cur_pcb->pid - 4;

    // remap + flush TLB 
    map_program(cur_pcb->pid);
    
    t_run = t_next;

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
    uint32_t freq = 100;
    uint32_t divisor = 1193182 / freq;
    // disable interrupts to set registers
    cli();
    // set command register - select channel 0, lobyte/hibyte, and rate generator - 0011 0110 
    outb(PIT_CMD, CMD_REG);
    // set high and low byte using outb (1 byte at a time)
    // outb(data, port)
    outb(divisor & LOW_8_BIT_MASK, CHANNEL_0);
    outb((divisor & HIGH_8_BIT_MASK) >> 8, CHANNEL_0);
    sti();
    // PIT is connected directly to IRQ0
    enable_irq(IRQ_PIT);
}
