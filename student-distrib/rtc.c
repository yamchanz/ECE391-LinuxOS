#include "idt_handlers.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"

void initialize_rtc() {
    // turn on IRQ8 - default 1024 Hz periodic frequency
    // cli();
    //disable NMI interrupt and select Register B
    outb(0x8B, 0x70);
    outb(0x8A, 0x70);
    // read last char from Register B
    char prev = inb(0x71);
    // set index again  
    outb(0x8B, 0x70);	
    // write previous value OR 0x40
    outb(prev | 0x40, 0x71);

    outb(0x8A, 0x70);
    outb(0x2F, 0x71);	

    // sti();

    // enable RTC on PIC
    enable_irq(8);

    return;
    
}
