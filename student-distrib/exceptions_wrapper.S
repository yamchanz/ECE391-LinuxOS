#define ASM 1
.globl divide_error_ex_handler, debug_ex_handler, nmi_interrupt_ex_handler
.globl breakpoint_ex_handler, overflow_ex_handler, bound_range_ex_handler
.globl invalid_opcode_ex_handler, device_not_avail_ex_handler, double_fault_ex_handler
.globl coprocess_seg_ex_handler, invalid_tss_ex_handler, seg_not_pres_ex_handler
.globl stack_fault_ex_handler, gen_prot_ex_handler, page_fault_ex_handler
.globl reserved_handler, fpu_fp_ex_handler, align_check_ex_handler
.globl machine_check_ex_handler, simd_fp_ex_handler



# divide_error_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
divide_error_ex_handler:
    pushal
    pushfl
    call divide_error_ex
    popfl
    popal
    iret

# debug_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
debug_ex_handler:
    pushal
    pushfl
    call debug_ex_handler
    popfl
    popal
    iret

# nmi_interrupt_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
nmi_interrupt_ex_handler:
    pushal
    pushfl
    call nmi_interrupt_ex
    popfl
    popal
    iret

# breakpoint_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
breakpoint_ex_handler:
    pushal
    pushfl
    call breakpoint_ex
    popfl
    popal
    iret

# overflow_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
overflow_ex_handler:
    pushal
    pushfl
    call overflow_ex
    popfl
    popal
    iret

# bound_range_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
bound_range_ex_handler:
    pushal
    pushfl
    call bound_range_ex
    popfl
    popal
    iret

# invalid_opcode_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
invalid_opcode_ex_handler:
    pushal
    pushfl
    call invalid_opcode_ex
    popfl
    popal
    iret

# device_not_avail_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
device_not_avail_ex_handler:
    pushal
    pushfl
    call device_not_avail_ex
    popfl
    popal
    iret

# double_fault_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
double_fault_ex_handler:
    pushal
    pushfl
    call double_fault_ex
    popfl
    popal
    iret

# coprocess_seg_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
coprocess_seg_ex_handler:
    pushal
    pushfl
    call coprocess_seg_ex
    popfl
    popal
    iret

# invalid_tss_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
invalid_tss_ex_handler:
    pushal
    pushfl
    call invalid_tss_ex
    popfl
    popal
    iret

# seg_not_pres_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
seg_not_pres_ex_handler:
    pushal
    pushfl
    call seg_not_pres_ex
    popfl
    popal
    iret

# stack_fault_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
stack_fault_ex_handler:
    pushal
    pushfl
    call stack_fault_ex
    popfl
    popal
    iret

# gen_prot_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
gen_prot_ex_handler:
    pushal
    pushfl
    call gen_prot_ex
    popfl
    popal
    iret

# page_fault_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
page_fault_ex_handler:
    pushal
    pushfl
    call page_fault_ex
    popfl
    popal
    iret

# reserved_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
reserved_ex_handler:
    pushal
    pushfl
    call reserved
    popfl
    popal
    iret

# fpu_fp_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
fpu_fp_ex_handler:
    pushal
    pushfl
    call fpu_fp_ex
    popfl
    popal
    iret

# align_check_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
align_check_ex_handler:
    pushal
    pushfl
    call align_check_ex
    popfl
    popal
    iret

# machine_check_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
machine_check_ex_handler:
    pushal
    pushfl
    call machine_check_ex
    popfl
    popal
    iret

# simd_fp_ex_handler
# DESCRIPTION: assembly linkage for exceptions
# FUNCTION: saves all regs, calls the exception, and then restores regs
simd_fp_ex_handler:
    pushal
    pushfl
    call simd_fp_ex
    popfl
    popal
    iret
