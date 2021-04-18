#define ASM 1
.globl sys_call_handler_link, context_switch, halt_ret

# sys_call_handler_link
# DESCRIPTION: assembly linkage for system calls interrupt handler
# FUNCTION: saves all regs, calls the handler, and then restores regs
sys_call_handler_link:
    # save registers to stack
    pushfl
    pushal

    # verify that system call number in EAX is valid (1-10)
    cmpl $0, %eax
    jle invalid_sys_call
    cmpl $10, %eax
    jg invalid_sys_call

    # valid, use jump table to call proper system call
    # parameters (3 args)
    pushl %edx
    pushl %ecx
    pushl %ebx
    call *sys_call_table(,%eax,4)
    movl %eax, save_eax # save the return val

    # pop parameters
    popl %ebx
    popl %ecx
    popl %edx
    jmp sys_call_done

# invalid system call number, return -1 and pop args and restore regs
invalid_sys_call:
    movl $-1, save_eax
sys_call_done:
    popal
    movl save_eax, %eax
    popfl
    iret

context_switch:
    # push USER_DS  0x2B
    xorl %eax, %eax
    movw $0x2B, %ax
    pushl %eax

    # push ESP - bottom of user stack
    pushl $0x8048000

    # push EFLAGS and enable interrupt
    pushfl
    orl $0x200, (%esp)

    # push CS - 0x23
    xorl %eax, %eax
    movw $0x23, %ax
    pushl %eax

    # push entry point
    pushl 20(%esp)

    # push IRET context to kernel stack
    iret

halt_ret:
    movl 4(%esp), %eax # status
    movl 8(%esp), %ebp # parent_pcb->ebp
    movl 12(%esp), %esp # parent_pcb->esp
    leave
    ret

sys_call_table:
    .long 0x0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

save_eax:
    .long 0x0
