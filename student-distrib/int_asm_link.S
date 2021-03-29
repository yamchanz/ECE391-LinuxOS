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
    pushal
    call sys_call_handler
    popal

    iret

# keyboard_handler_link
# DESCRIPTION: assembly linkage for keyboard interrupt handler
# FUNCTION: saves all regs, calls the handler, and then restores regs
keyboard_handler_link:
    pushal
    call keyboard_handler
    popal

    iret

/*jumptable:
    .long 0x0
    .long sys_call_read
    .long sys_call
*/