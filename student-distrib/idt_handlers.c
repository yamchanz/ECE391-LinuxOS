#include "idt_handlers.h"
#include "x86_desc.h"

/* Array of exception functions */
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
void fpu_fp_ex();
void align_check_ex();
void machine_check_ex();
void simd_fp_ex();

void (*exception_arr[20])() = {divide_error_ex, debug_ex, nmi_interrupt_ex, breakpoint_ex, overflow_ex,
                                bound_range_ex, invalid_opcode_ex, device_not_avail_ex, double_fault_ex, coprocess_seg_ex,
                                invalid_tss_ex, seg_not_pres_ex, stack_fault_ex, gen_prot_ex, page_fault_ex, NULL,
                                fpu_fp_ex, align_check_ex, machine_check_ex, simd_fp_ex};

/* Initialize the IDT */
void initialize_idt() {
    int i;
    for(i = 0; i < EX_ARR_SIZE; i++) {
        // skip intel reserved interrupt 15
        if(i == EXCEPTION_15) {
            continue;
        }
        install_interrupt_handler(i, exception_arr[i]);
    }
}




/* TYPE 1: Task-gate descriptor */
static void install_task_handler(int idt_offset, void (*handler)) {
    // pass in pointer to handler function

    // set DPL to 0 for hardware handlers to prevent callings with int
    idt[idt_offset].__attribute__.dpl = 0;

    // set gate type - 32 bit task gate
    idt[idt_offset].__attribute__.reserved0 = 1;
    idt[idt_offset].__attribute__.reserved1 = 0;
    idt[idt_offset].__attribute__.reserved2 = 1;
    idt[idt_offset].__attribute__.reserved3 = 0;
    idt[idt_offset].__attribute__.reserved4 = 0;

    // set to 1 for used interrupts
    idt[idt_offset].__attribute__.present = 1;

    // set to 0 for interrupt/traps
    idt[idt_offset].__attribute__.size = 1;

    // write the 
    SET_IDT_ENTRY(idt[idt_offset].__attribute__, handler);

    return;
}

/* TYPE 2: Interrupt-gate descriptor */
static void install_interrupt_handler(int idt_offset, void (*handler)) {
    // set values of idt_descriptor struct

    // set gate type - 32 bit interrupt gate
    idt[idt_offset].__attribute__.reserved0 = 0;
    idt[idt_offset].__attribute__.reserved1 = 1;
    idt[idt_offset].__attribute__.reserved2 = 1;
    idt[idt_offset].__attribute__.reserved3 = 1;
    idt[idt_offset].__attribute__.reserved4 = 1;

    // set to 1 for used interrupts
    idt[idt_offset].__attribute__.present = 1;

    // set to 0 for interrupt/traps
    idt[idt_offset].__attribute__.size = 0;

    // set DPL to 0 for hardware handlers to prevent callings with int
    idt[idt_offset].__attribute__.dpl = 0;

    // write the 
    SET_IDT_ENTRY(idt[idt_offset].__attribute__, handler);

    return;
}

/* TYPE 3: Trap-gate descriptor */
static void install_trap_handler(int idt_offset, void (*handler)) {
    // set values of idt_descriptor struct

    // set gate type - 32 bit trap gate
    idt[idt_offset].__attribute__.reserved0 = 1;
    idt[idt_offset].__attribute__.reserved1 = 1;
    idt[idt_offset].__attribute__.reserved2 = 1;
    idt[idt_offset].__attribute__.reserved3 = 1;
    idt[idt_offset].__attribute__.reserved4 = 1;

    // set to 1 for used interrupts
    idt[idt_offset].__attribute__.present = 1;

    // set to 0 for interrupt/traps
    idt[idt_offset].__attribute__.size = 0;

    // set DPL to 0 for hardware handlers to prevent callings with int
    idt[idt_offset].__attribute__.dpl = 0;

    // write the 
    SET_IDT_ENTRY(idt[idt_offset].__attribute__, handler);

    return;
}

/* Exception handler functions */
void divide_error_ex() {
    printf("Divide Error Exception (#DE)");
    // freeze with while loop
    while(1) {

    }
}

void debug_ex() {
    printf("Debug Exception (#DB)");
    // freeze with while loop
    while(1) {

    }
}

void nmi_interrupt_ex() {
    printf("NMI Interrupt");
    // freeze with while loop
    while(1) {

    }
}

void breakpoint_ex() {
    printf("Breakpoint Exception (#BP)");
    // freeze with while loop
    while(1) {

    }
}

void overflow_ex() {
    printf("Overflow Exception (#OF)");
    // freeze with while loop
    while(1) {

    }
}

void bound_range_ex() {
    printf("BOUND Range Exceeded Exception (#BR)");
    // freeze with while loop
    while(1) {

    }
}

void invalid_opcode_ex() {
    printf("Invalid Opcode Exception (#UD)");
    // freeze with while loop
    while(1) {

    }
}

void device_not_avail_ex() {
    printf("Device Not Available Exception (#NM)");
    // freeze with while loop
    while(1) {

    }
}

void double_fault_ex() {
    printf("Double Fault Exception (#DF)");
    // freeze with while loop
    while(1) {

    }
}

void coprocess_seg_ex() {
    printf("Coprocessor Segment Overrun");
    // freeze with while loop
    while(1) {

    }
}

void invalid_tss_ex() {
    printf("Invalid TSS Exception (#TS)");
    // freeze with while loop
    while(1) {

    }
}

void seg_not_pres_ex() {
    printf("Segment Not Present (#NP)");
    // freeze with while loop
    while(1) {

    }
}

void stack_fault_ex() {
    printf("Stack Fault Exception (#SS)");
    // freeze with while loop
    while(1) {

    }
}

void gen_prot_ex() {
    printf("General Protection Exception (#GP)");
    // freeze with while loop
    while(1) {

    }
}

void page_fault_ex() {
    printf("Page-Fault Exception (#PF)");
    // freeze with while loop
    while(1) {

    }
}

void fpu_fp_ex() {
    printf("FPU Floating-Point Error (#MF)");
    // freeze with while loop
    while(1) {

    }
}

void align_check_ex() {
    printf("Alignment Check Exception (#AC)");
    // freeze with while loop
    while(1) {

    }
}

void machine_check_ex() {
    printf("Machine-Check Exception (#MC)");
    // freeze with while loop
    while(1) {

    }
}

void simd_fp_ex() {
    printf("SIMD Floating-Point Exception (#XF)");
    // freeze with while loop
    while(1) {

    }
}
