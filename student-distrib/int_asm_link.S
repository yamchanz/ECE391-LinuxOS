.text
# Assembly linkage for interrupt handlers (RTC, system call, keyboard, etc.)
# used to save registers before calling handler

.globl rtc_handler_link, keyboard_handler_link, pit_handler_link

# rtc_handler_link
# DESCRIPTION: assembly linkage for RTC interrupt handler
# FUNCTION: saves all regs, calls the handler, and then restores regs
rtc_handler_link:
    pushal
    pushfl
    call rtc_handler
    popfl
    popal
    iret

# keyboard_handler_link
# DESCRIPTION: assembly linkage for keyboard interrupt handler
# FUNCTION: saves all regs, calls the handler, and then restores regs
keyboard_handler_link:
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    iret

pit_handler_link:
    pushal
    pushfl
    call pit_handler
    popfl
    popal
    iret