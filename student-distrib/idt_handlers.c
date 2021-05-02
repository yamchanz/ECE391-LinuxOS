#include "idt_handlers.h"
#include "int_asm_link.h"
#include "x86_desc.h"
#include "i8259.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "scheduler.h"

/* Array of exception functions (0x00 to 0x13) */
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
void gen_prot_ex();;
void page_fault_ex();
// 15 reserved by intel
void reserved();
void fpu_fp_ex();
void align_check_ex();
void machine_check_ex();
void simd_fp_ex();
// 0x80 system call handler
void sys_call_handler();
// undefined general handler
void undefined_handler();

void (*exception_arr[20])() = {divide_error_ex, debug_ex, nmi_interrupt_ex, breakpoint_ex, overflow_ex,
                                bound_range_ex, invalid_opcode_ex, device_not_avail_ex, double_fault_ex, coprocess_seg_ex,
                                invalid_tss_ex, seg_not_pres_ex, stack_fault_ex, gen_prot_ex, page_fault_ex, reserved,
                                fpu_fp_ex, align_check_ex, machine_check_ex, simd_fp_ex};

/* initialize_idt
    DESCRIPTION: initializes the Interrupt-Descriptor Table created in x86_desc with interrupt handlers for exceptions and the i8259 PIC
    INPUTS: none
    OUTPUTS: populates idt defined in x86_desc with idt_desc structs that are handlers for interrupts and exceptions
    RETURN VALUE: none
    SIDE EFFECTS: none
*/
void initialize_idt() {
    // install first 20 intel exceptions
    int i;
    for(i = 0; i < EX_ARR_SIZE; i++) {
        install_interrupt_handler(i, exception_arr[i], 0, 0);
    }
    // initialize the rest of IDT table with a general handler, later installs will install over this
    int j;
    for(j = EX_ARR_SIZE; j < NUM_VEC; j++) {
        install_interrupt_handler(j, &undefined_handler, 0, 0);
    }

    // install RTC (IRQ8)
    install_interrupt_handler(RTC_IDX, rtc_handler_link, 0, 0);
    // install keyboard (IRQ1)
    install_interrupt_handler(KEYBOARD_IDX, keyboard_handler_link, 0, 0);
    // install PIT (IRQ0)
    install_interrupt_handler(PIT_IDX, pit_handler_link, 0, 0);
    // system call handler (0x80)
    install_interrupt_handler(SYS_CALL_IDX, sys_call_handler_link, 0, 1);

    // load IDT using desc pointer containing base address
    lidt(idt_desc_ptr);

}

/* TYPE 2: Interrupt-gate descriptor */
/* install_interrupt_handler
    DESCRIPTION: installs an interrupt/trap handler into the IDT
    INPUTS: idt_offset: index of the interrupt in the IDT
            handler: interrupt handler function
            trap: 0 or 1 to determine if installing interrupt or trap handler
            sys_call: 0 or 1 to determine DPL for system call
    OUTPUTS: populates idt with an idt_desc at that offset
    RETURN VALUE: none
    SIDE EFFECTS: none
*/
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

/* pit_handler
    DESCRIPTION: installs the interrupt handler for the PIT
    INPUTS: none
    OUTPUTS: writes to PIT registers
    RETURN VALUE: none
    SIDE EFFECTS: none
*/
void pit_handler() {
    // issue EOI to PIC at end of interrupt
    send_eoi(IRQ_PIT);
    // get pcb of current process
    // pcb_t* old_process = get_pcb(t[t_run].running_process);
    // prepare for switching process

    // begin scheduling
    schedule();
}

/* rtc_handler
    DESCRIPTION: installs the interrupt handler for the RTC
    INPUTS: none
    OUTPUTS: writes to RTC registers
    RETURN VALUE: none
    SIDE EFFECTS: none
*/
void rtc_handler() {
    // disable interrupts to set registers and flags
    cli();
    /* read from register C - if not then interrupt will not happen again.
        We select, read from reg C and then throw away the contents immediately.  */
    outb(REG_C, SELECT_REG);
    inb(DATA_REG);

    // update volatile flag while interrupts disabled
    if(!rtc_int_received) {
        rtc_int_received = 1;
    }

    // rtc test for CP1
	// test_interrupts();

    // issue EOI to PIC at end of interrupt
    send_eoi(IRQ_RTC);
}

/* sys_call_handler
    DESCRIPTION: installs the system call handler (0x80) (will have functionality implemented later CPs)
    INPUTS: none
    OUTPUTS: none
    RETURN VALUE: none
    SIDE EFFECTS: none
*/
void sys_call_handler() { // no longer used
    // clear();
    // printf("System call was handled");
    // // freeze with while loop
    // while(1) {

    // }
}

/* Exception handler functions
    DESCRIPTION: handler functions for each of the first 20 interrupts, following the Intel IA-32 manual
*/
/*  divide_error_ex
    DESCRIPTION: handler functions for divide by zero exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void divide_error_ex() {
    printf("Divide Error Exception (#DE)");
    exception_flag = 1;
    halt(1);
}
/*  debug_ex
    DESCRIPTION: handler functions for debug exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void debug_ex() {
    printf("Debug Exception (#DB)");
    exception_flag = 1;
    halt(1);
}
/*  nmi_interrupt_ex
    DESCRIPTION: handler functions for NMI interrupt exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void nmi_interrupt_ex() {
    printf("NMI Interrupt");
    exception_flag = 1;
    halt(1);
}
/*  breakpoint_ex
    DESCRIPTION: handler functions for breakpoint exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void breakpoint_ex() {
    printf("Breakpoint Exception (#BP)");
    exception_flag = 1;
    halt(1);
}
/*  overflow_ex
    DESCRIPTION: handler functions for overflow exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void overflow_ex() {
    printf("Overflow Exception (#OF)");
    exception_flag = 1;
    halt(1);
}
/*  bound_range_ex
    DESCRIPTION: handler functions for bound range exceeded exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void bound_range_ex() {
    printf("BOUND Range Exceeded Exception (#BR)");
    exception_flag = 1;
    halt(1);
}
/*  invalid_opcode_ex
    DESCRIPTION: handler functions for invalid opcode exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void invalid_opcode_ex() {
    printf("Invalid Opcode Exception (#UD)");
    exception_flag = 1;
    halt(1);
}
/*  device_not_avail_ex()
    DESCRIPTION: handler functions for device not available exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void device_not_avail_ex() {
    printf("Device Not Available Exception (#NM)");
    exception_flag = 1;
    halt(1);
}
/*  double_fault_ex
    DESCRIPTION: handler functions for double fault exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void double_fault_ex() {
    printf("Double Fault Exception (#DF)");
    exception_flag = 1;
    halt(1);
}
/*  coprocess_seg_ex
    DESCRIPTION: handler functions for coprocessor segment overrun exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void coprocess_seg_ex() {
    printf("Coprocessor Segment Overrun");
    exception_flag = 1;
    halt(1);
}
/*  invalid_tss_ex
    DESCRIPTION: handler functions for invalid TSS exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void invalid_tss_ex() {
    clear();
    printf("Invalid TSS Exception (#TS)");
    exception_flag = 1;
    halt(1);
}
/*  seg_not_pres_ex
    DESCRIPTION: handler functions for segment not present exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void seg_not_pres_ex() {
    clear();
    printf("Segment Not Present (#NP)");
    exception_flag = 1;
    halt(1);
}
/*  stack_fault_ex
    DESCRIPTION: handler functions for stack fault exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void stack_fault_ex() {
    clear();
    printf("Stack Fault Exception (#SS)");
    exception_flag = 1;
    halt(1);
}
/*  gen_prot_ex
    DESCRIPTION: handler functions for general protection exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void gen_prot_ex() {
    clear();
    printf("General Protection Exception (#GP)");
    exception_flag = 1;
    halt(1);
}
/*  page_fault_ex
    DESCRIPTION: handler functions for page fault exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void page_fault_ex() {
    clear();
    printf("Page-Fault Exception (#PF)");
    exception_flag = 1;
    halt(1);
}
/*  reserved
    DESCRIPTION: general handler function for interrupts reserved by Intel
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void reserved(){
    clear();
    printf("Reserved by Intel");
    exception_flag = 1;
    halt(1);
}
/*  fpu_fp_ex
    DESCRIPTION: handler functions for FPU floating point error exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void fpu_fp_ex() {
    clear();
    printf("FPU Floating-Point Error (#MF)");
    exception_flag = 1;
    halt(1);
}
/*  align_check_ex
    DESCRIPTION: handler functions for alignment check exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void align_check_ex() {
    clear();
    printf("Alignment Check Exception (#AC)");
    exception_flag = 1;
    halt(1);
}
/*  machine_check_ex
    DESCRIPTION: handler functions for machine-check exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void machine_check_ex() {
    clear();
    printf("Machine-Check Exception (#MC)");
    exception_flag = 1;
    halt(1);
}
/*  simd_fp_ex
    DESCRIPTION: handler functions for SIMD floating point exception
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void simd_fp_ex() {
    clear();
    printf("SIMD Floating-Point Exception (#XF)");
    exception_flag = 1;
    halt(1);
}
/* undefined_handler
    DESCRIPTION: handler function for interrupts not yet defined otherwise
    INPUTS: none
    OUTPUTS: writes the interrupt to the screen, and then freezes with a while loop
    RETURN VALUE: none
    SIDE EFFECTS: freezes the kernel
*/
void undefined_handler() {
    clear();
    printf("General handler implemented");
    exception_flag = 1;
    halt(1);
}
