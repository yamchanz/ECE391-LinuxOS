#define ASM 1
.globl divide_error_ex_handler, debug_ex_handler, nmi_interrupt_ex_handler, 
    breakpoint_ex_handler, overflow_ex_handler, bound_range_ex_handler, 
    invalid_opcode_ex_handler, device_not_avail_ex_handler, double_fault_ex_handler, 
    coprocess_seg_ex_handler, invalid_tss_ex_handler, seg_not_pres_ex_handler, 
    stack_fault_ex_handler, gen_prot_ex_handler, page_fault_ex_handler, 
    reserved_handler, fpu_fp_ex_handler, align_check_ex_handler, 
    machine_check_ex_handler, simd_fp_ex_handler

# sys_call_handler_link
# DESCRIPTION: assembly linkage for system calls interrupt handler
# FUNCTION: saves all regs, calls the handler, and then restores regs
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret

debug_ex_handler:
    pushal
    pushfl
    call debug_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex_handler
    popfl
    popal
    iret
