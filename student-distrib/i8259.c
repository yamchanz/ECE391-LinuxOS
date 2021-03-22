/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
static const uint8_t master_mask = 0xFF; /* IRQs 0-7  */
static const uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */
/* formats of the enable_irq, disable_irq is referenced from the osdev.org */

/* i8259_init
    DESCRIPTION: Initialize the 8259 PIC
    INPUTS: none
    OUTPUTS: none
    RETURN VALUE: none
    SIDE EFFECTS: masks the interrupts before and after the initialization is complete, sends four ICWs for intializing
*/
void i8259_init(void) {
    outb(master_mask, MASTER_8259_DATA); // initially mask all the interrupts
    outb(slave_mask, SLAVE_8259_DATA);  
    outb(ICW1,MASTER_8259_PORT);        //send the first intialization word to master
    outb(ICW1,SLAVE_8259_PORT);

    outb(ICW2_MASTER,MASTER_8259_DATA); //send the second intialization word to master
    outb(ICW2_SLAVE,SLAVE_8259_DATA);

    outb(ICW3_MASTER,MASTER_8259_DATA); //send the third intialization word to master
    outb(ICW3_SLAVE,SLAVE_8259_DATA);

    outb(ICW4,MASTER_8259_DATA);        //send the fourth intialization word to master
    outb(ICW4,SLAVE_8259_DATA);

    outb(master_mask, MASTER_8259_DATA);    // making sure all the interrupts are masked until initialization is complete
    outb(slave_mask, SLAVE_8259_DATA);

    enable_irq(SLAVE);
}


/* enable_irq
    DESCRIPTION: Enable (unmask) the specified IRQ 
    INPUTS: irq_num(the irq pin number of which the interrupt is raisied)
    OUTPUTS: none
    RETURN VALUE: none
    SIDE EFFECTS: enables the irq on the corresponding PIC's irq line; unmasking
*/
void enable_irq(uint32_t irq_num) {
    uint16_t port;  // port at which we will enable the irq
    uint8_t value;  // the irq line value

    if(irq_num < MAX_PIN ){    // if the interrupt happens at the master
        port = MASTER_8259_DATA;    // then the port we need to read and write is master
    } else {
        port = SLAVE_8259_DATA; // if the interrupt happens at the slave then the port we need to read and write is master
        irq_num -= MAX_PIN;    // subtract by maximum pin number to make sure the interrupt request number lies within 0 to 7 range
    }
    value = inb(port) & ~(1 << irq_num); // we enable the corresponding bit of the irq number requested by shifting 1 by the irq number and doing logically AND with the existing IRQ line
    outb(value, port);                  // and we rewrite to the corresponding port of the interrupt
}


/* disable_irq
    DESCRIPTION: Disable (mask) the specified IRQ  
    INPUTS: irq_num(the irq pin number of which the interrupt is raisied)
    OUTPUTS: none
    RETURN VALUE: none
    SIDE EFFECTS: disables the irq on the corresponding PIC's irq line; masking
*/
void disable_irq(uint32_t irq_num) {
    uint16_t port;  // port at which we will enable the irq
    uint8_t value;  // the irq line value

    if(irq_num < MAX_PIN ){     // if the interrupt happens at the master
        port = MASTER_8259_DATA;    // then the port we need to read and write is master
    } else {
        port = SLAVE_8259_DATA; // if the interrupt happens at the slave then the port we need to read and write is master
        irq_num -= MAX_PIN; // subtract by maximum pin number to make sure the interrupt request number lies within 0 to 7 range
    }
    value = inb(port) | (1 << irq_num); // we disable by setting the corresponding bit on the irq line 1 and performing logical OR with the existing IRQ line
    outb(value, port);  // and we rewrite to the corresponding port of the interrupt
}


/* send_eoi
    DESCRIPTION: Send end-of-interrupt signal for the specified IRQ 
    INPUTS: irq_num(the irq pin number of which the interrupt is raisied) and of which the interrupt handling is finished
    OUTPUTS: none
    RETURN VALUE: none
    SIDE EFFECTS: sends the signal that the interrupt is completed so the processor can handle the next if there is any
*/
void send_eoi(uint32_t irq_num) {
    if(irq_num >= MAX_PIN){ // the interrupt happens at the slave PIC
        outb((irq_num - MAX_PIN) | EOI,SLAVE_8259_COMMAND );    // if the interrupt happened at the slave PIC we need to send EOI to both slave
        outb(SLAVE | EOI,MASTER_8259_COMMAND );                 // and master
    } else {
        outb(irq_num | EOI,MASTER_8259_COMMAND );               // if the interrupt happened at the master, we send the EOI to the master port
    }
}

