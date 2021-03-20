#include "idt_handlers.h"
#include "x86_desc.h"
#include "lib.h"

void initialize_rtc() {
    // disable interrupts during RTC
    cli();
    // disable NMI interrupt, select register A
    outb(0x70, 0x8A);
    // write to CMOS/RTC RAM
    outb(0x71, 0x20);
    // reenable interrupts
    sti();


    
}