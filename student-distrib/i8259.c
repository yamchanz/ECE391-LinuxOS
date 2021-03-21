/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
<<<<<<< HEAD
    outb(master_mask, MASTER_8259_DATA); 
    outb(slave_mask, SLAVE_8259_DATA);

=======
>>>>>>> hyunjun
    outb(ICW1,MASTER_8259_PORT);
    outb(ICW1,SLAVE_8259_PORT);

    outb(ICW2_MASTER,MASTER_8259_DATA);
    outb(ICW2_SLAVE,SLAVE_8259_DATA);

    outb(ICW3_MASTER,MASTER_8259_DATA);
    outb(ICW3_SLAVE,SLAVE_8259_DATA);

    outb(ICW4,MASTER_8259_DATA);
    outb(ICW4,SLAVE_8259_DATA);

<<<<<<< HEAD
    enable_irq(2);
=======
>>>>>>> hyunjun
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;
    uint32_t IRQ_LINE;

    if(irq_num <8 ){
        port = MASTER_8259_DATA;
    } else {
        port = SLAVE_8259_DATA;
        IRQ_LINE -= 8;
    }
    value = inb(port) & ~(1 << IRQ_LINE);
    outb(port, value);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;
    uint32_t IRQ_LINE;

    if(irq_num <8 ){
        port = MASTER_8259_DATA;
    } else {
        port = SLAVE_8259_DATA;
        IRQ_LINE -= 8;
    }
    value = inb(port) | (1 << IRQ_LINE);
    outb(port, value);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8){
        outb((irq_num - 8) | EOI,SLAVE_8259_COMMAND );
        outb(2 | EOI,MASTER_8259_COMMAND );
    } else {
        outb(irq_num | EOI,MASTER_8259_COMMAND );
    }
}

