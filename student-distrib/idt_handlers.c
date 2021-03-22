#include "idt_handlers.h"
#include "int_asm_link.h"
#include "x86_desc.h"
#include "i8259.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"

/* Array of exception functions (0x00 to 0x14) */
void divide_error_ex();
void debug_ex();
void nmi_interrupt_ex();
void breakpoint_ex();
void overflow_ex();
void bound_range_ex();
void invalid_opcode_ex();
void device_not_avail_ex();
void double_fault_ex();
void coprocess_seg_ex();
void invalid_tss_ex();
void seg_not_pres_ex();
void stack_fault_ex();
void gen_prot_ex();
void page_fault_ex();
// 15 reserved by intel
void reserved();
void fpu_fp_ex();
void align_check_ex();
void machine_check_ex();
void simd_fp_ex();

void sys_call_handler();

void (*exception_arr[20])() = {divide_error_ex, debug_ex, nmi_interrupt_ex, breakpoint_ex, overflow_ex,
                                bound_range_ex, invalid_opcode_ex, device_not_avail_ex, double_fault_ex, coprocess_seg_ex,
                                invalid_tss_ex, seg_not_pres_ex, stack_fault_ex, gen_prot_ex, page_fault_ex, reserved,
                                fpu_fp_ex, align_check_ex, machine_check_ex, simd_fp_ex};

/* Initialize the IDT */
void initialize_idt() {
    int i;
    for(i = 0; i < EX_ARR_SIZE; i++) {
        install_interrupt_handler(i, exception_arr[i], 0, 0);
    }

    // install RTC (IRQ8)
    install_interrupt_handler(0x28, rtc_handler_link, 0, 0);

    // install_interrupt_handler(0x21, keyboard_handler_link, 0, 0);

    // system call handler (0x80)
    install_interrupt_handler(SYS_CALL_IDX, sys_call_handler_link, 0, 1);

    // load IDT using desc pointer containing base address
    lidt(idt_desc_ptr);

}

/* TYPE 2: Interrupt-gate descriptor */
void install_interrupt_handler(int idt_offset, void (*handler), int trap, int sys_call) {
    // set values of idt_descriptor struct

    // set gate type - 32 bit interrupt gate
    idt[idt_offset].reserved0 = 0;
    idt[idt_offset].reserved1 = 1;
    idt[idt_offset].reserved2 = 1;
    idt[idt_offset].reserved3 = trap;
    idt[idt_offset].reserved4 = 0;
    // kernel selector
    idt[idt_offset].seg_selector = KERNEL_CS;
    // set to 1 for used interrupts
    idt[idt_offset].present = 1;
    idt[idt_offset].size = 1;
    // set DPL to 0 for hardware handlers to prevent callings with int
    if(!sys_call) {
        idt[idt_offset].dpl = 0;
    } else {
        // accessible from use space space via int
        idt[idt_offset].dpl = 3;
    }

    SET_IDT_ENTRY(idt[idt_offset], handler);

    return;
}

void rtc_handler() {
    // disable interrupts to set registers
    // cli();
    /* read from register C - if not then interrupt will not happen again.
        We select, read from reg C and then throw away the contents immediately.  */
    outb(0x0C, 0x70);
    inb(0x71);
    // sti();

    // rtc test
	test_interrupts();

    // issue EOI to PIC at end of interrupt
    send_eoi(8);
}


void sys_call_handler() {
    printf("System call was handled");
}

/* Exception handler functions */
void divide_error_ex() {
    clear();
    printf("Divide Error Exception (#DE)");
    // freeze with while loop
    while(1) {

    }
}

void debug_ex() {
    clear();
    printf("Debug Exception (#DB)");
    // freeze with while loop
    while(1) {

    }
}

void nmi_interrupt_ex() {
    clear();
    printf("NMI Interrupt");
    // freeze with while loop
    while(1) {

    }
}

void breakpoint_ex() {
    clear();
    printf("Breakpoint Exception (#BP)");
    // freeze with while loop
    while(1) {

    }
}

void overflow_ex() {
    clear();
    printf("Overflow Exception (#OF)");
    // freeze with while loop
    while(1) {

    }
}

void bound_range_ex() {
    clear();
    printf("BOUND Range Exceeded Exception (#BR)");
    // freeze with while loop
    while(1) {

    }
}

void invalid_opcode_ex() {
    clear();
    printf("Invalid Opcode Exception (#UD)");
    // freeze with while loop
    while(1) {

    }
}

void device_not_avail_ex() {
    clear();
    printf("Device Not Available Exception (#NM)");
    // freeze with while loop
    while(1) {

    }
}

void double_fault_ex() {
    clear();
    printf("Double Fault Exception (#DF)");
    // freeze with while loop
    while(1) {

    }
}

void coprocess_seg_ex() {
    clear();
    printf("Coprocessor Segment Overrun");
    // freeze with while loop
    while(1) {

    }
}

void invalid_tss_ex() {
    clear();
    printf("Invalid TSS Exception (#TS)");
    // freeze with while loop
    while(1) {

    }
}

void seg_not_pres_ex() {
    clear();
    printf("Segment Not Present (#NP)");
    // freeze with while loop
    while(1) {

    }
}

void stack_fault_ex() {
    clear();
    printf("Stack Fault Exception (#SS)");
    // freeze with while loop
    while(1) {

    }
}

void gen_prot_ex() {
    clear();
    printf("General Protection Exception (#GP)");
    // freeze with while loop
    while(1) {

    }
}

void page_fault_ex() {
    clear();
    printf("Page-Fault Exception (#PF)");
    // freeze with while loop
    while(1) {

    }
}

void reserved(){
    clear();
    printf("Reserved by Intel");
    // freeze with while looop
    while(1) {
        
    }
}

void fpu_fp_ex() {
    clear();
    printf("FPU Floating-Point Error (#MF)");
    // freeze with while loop
    while(1) {

    }
}

void align_check_ex() {
    clear();
    printf("Alignment Check Exception (#AC)");
    // freeze with while loop
    while(1) {

    }
}

void machine_check_ex() {
    clear();
    printf("Machine-Check Exception (#MC)");
    // freeze with while loop
    while(1) {

    }
}

void simd_fp_ex() {
    clear();
    printf("SIMD Floating-Point Exception (#XF)");
    // freeze with while loop
    while(1) {

    }
}
