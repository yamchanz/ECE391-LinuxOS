.data
# Assembly linkage for interrupt handlers (RTC, system call, keyboard, etc.)
# used to save registers before calling handler

.globl rtc_handler_link, sys_call_handler_link, keyboard_handler_link

# save all registers, call function and then restore all 
rtc_handler_link:
    pushal
    call rtc_handler
    popal

    iret

sys_call_handler_link:
    pushal
    call sys_call_handler
    popal

    iret

keyboard_handler_link:
    pushal
    call keyboard_handler
    popal

    iret