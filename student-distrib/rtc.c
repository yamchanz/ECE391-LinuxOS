#include "idt_handlers.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"

// flag to set if interrupt has been received - used for reading only after interrupt
int rtc_int_received;

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
    // disable NMI interrupt and select Registers
    outb(NMI_REG_B, SELECT_REG);
    outb(NMI_REG_A, SELECT_REG);
    // read last char from Register B
    char prev = inb(DATA_REG);
    // set index again  
    outb(NMI_REG_B, SELECT_REG);	
    // OR to turn on 6th bit of register B
    outb(prev | BIT_6_MASK, DATA_REG);

    // sti();

    rtc_int_received = 0;

    // enable RTC on PIC
    enable_irq(IRQ_RTC);

    return;
    
}

/* rtc_open
    DESCRIPTION: initializes RTC by enabling on PIC, then sets frequency to 2Hz
    INPUTS: filename (unused)
    OUTPUTS: sets frequency in Register A to 2Hz, enables IRQ8 on PIC
    RETURN VALUE: 0 on success
    SIDE EFFECTS: none
*/
int32_t rtc_open(const uint8_t* filename) {
    initialize_rtc();

    outb(NMI_REG_A, SELECT_REG);
    char prev = inb(DATA_REG);
    // set frequency to 2Hz
    outb(NMI_REG_A, SELECT_REG);
    outb((prev & HIGH_BIT_MASK) | TWO_HZ_FREQ, DATA_REG);

    return 0;
}

/* rtc_read
    DESCRIPTION: blocks until interrupt has been received through RTC interrupt flag
    INPUTS: fd, buf, nbytes (all unused)
    OUTPUTS: none
    RETURN VALUE: 0 on success
    SIDE EFFECTS: none
*/
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    while(!rtc_int_received) {
        // block until the next interrupt
    }
    rtc_int_received = 0;

    return 0;
}

/* rtc_write
    DESCRIPTION: sets the RTC to a certain frequency, with error checking
    INPUTS: fd (unused)
            buf - buffer that contains desired frequency
            nbytes - needs to accept 4 byte rate
    OUTPUTS: enables IRQ8 on the PIC, writes to the RTC registers
    RETURN VALUE: number of bytes written, -1 on fail
    SIDE EFFECTS: none
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    uint32_t freq = *(const uint32_t*)buf;
    if(freq == NULL) {
        return -1;
    }
    /* check if power of two - referenced formula from 
    https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2 */
    if(freq && (!(freq & (freq - 1))) == 0) {
        return -1;
    }
    // sys call only accepts 4 byte int
    if(nbytes == INT_BYTES && freq <= HIGH_LIMIT_FREQ) {
        // count power of 2 by finding where the 1 bit is
        int pow = 0;
        while(freq >>= 1) { 
            ++pow;
        }
        // write new frequency into Register A
        // disable NMI interrupt and select Register A
        outb(NMI_REG_A, SELECT_REG);
        char prev = inb(DATA_REG);
        // set index again
        outb(NMI_REG_A, SELECT_REG);
        // set rate based on offset from 2Hz rate
        outb((prev & HIGH_BIT_MASK) | (TWO_HZ_FREQ - pow - 1), DATA_REG);
    } else {
        return -1;
    }

    // return number of bytes written on success
    return nbytes;
}

/* rtc_close
    DESCRIPTION: closes RTC and makes it available from later calls to open
    INPUTS: fd (unused)
    OUTPUTS: none
    RETURN VALUE: none
    SIDE EFFECTS: none
*/
int32_t rtc_close(int32_t fd) {
    return 0;
}


