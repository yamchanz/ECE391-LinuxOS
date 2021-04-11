.data
# Assembly linkage for interrupt handlers (RTC, system call, keyboard, etc.)
# used to save registers before calling handler

.globl rtc_handler_link, sys_call_handler_link, keyboard_handler_link

# rtc_handler_link
# DESCRIPTION: assembly linkage for RTC interrupt handler
# FUNCTION: saves all regs, calls the handler, and then restores regs
rtc_handler_link:
    pushal
    call rtc_handler
    popal

    iret

# sys_call_handler_link
# DESCRIPTION: assembly linkage for system calls interrupt handler
# FUNCTION: saves all regs, calls the handler, and then restores regs
sys_call_handler_link:
    # callee save registers to stack
    pushl %ebp
    pushl %edi
    pushl %esi

    # parameters (3 args)
    pushl %edx
    pushl %ecx
    pushl %ebx

    # verify that system call number in EAX is valid (1-10, zero indexed)
    cmpl $0, %eax
    jl invalid_sys_call
    cmpl $10, %eax
    jge invalid_sys_call
    # valid, use jump table to call proper system call
    call *sys_call_table(, %eax, 4)
    # return code is in eax. pop parameters
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp

    iret

    invalid_sys_call:
    # invalid system call number, return -1 and pop args and restore regs
    movl $-1, eax
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp

    iret

# keyboard_handler_link
# DESCRIPTION: assembly linkage for keyboard interrupt handler
# FUNCTION: saves all regs, calls the handler, and then restores regs
keyboard_handler_link:
    pushal
    call keyboard_handler
    popal

    iret

sys_call_table:
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
