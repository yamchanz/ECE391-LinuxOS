#include "idt_handlers.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"

/* initialize_rtc
    DESCRIPTION: initializes RTC by setting the default frequency and enabling on the PIC
    INPUTS: none
    OUTPUTS: enables IRQ8 on the PIC, writes to the RTC registers
    RETURN VALUE: none
    SIDE EFFECTS: none
*/
void initialize_rtc() {
    // turn on IRQ8 - default 1024 Hz periodic frequency
    // cli();
    //disable NMI interrupt and select Register B
    outb(NMI_REG_B, SELECT_REG);
    // outb(NMI_REG_A, SELECT_REG);
    // read last char from Register B
    char prev = inb(DATA_REG);
    // set index again  
    outb(NMI_REG_B, SELECT_REG);	
    // OR to turn on 6th bit of register B
    outb(prev | 0x40, DATA_REG);

    // outb(NMI_REG_A, SELECT_REG);
    // outb(0x2F, DATA_REG);	

    // sti();

    // enable RTC on PIC
    enable_irq(IRQ_RTC);

    return;
    
}
