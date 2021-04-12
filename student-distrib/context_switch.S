.data
# Assembly linkage for interrupt handlers (RTC, system call, keyboard, etc.)
# used to save registers before calling handler

.globl context_switch
# reference http://www.jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
context_switch:
    # get parameters (esp, entry_point)
    cli
    movl 4(%esp), %ecx # esp
    movl 8(%esp), %edx # entry_point

    # push USER_DS                    
    xorl %eax, %eax               
    movw $0x0023, %ax  
    movw %ax, %ds  
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs            
    pushl %eax                               
                                    
    # push ESP                      
    pushl %ecx                        
                                    
    # push EFLAGS                   
    pushfl     

    popl %eax
    orl $0x200, %eax   
    pushl %eax

    # push CS - 0x2B (43)           
    xorl %eax, %eax               
    movw $0x002B, %ax                 
    pushl %eax                           

    # push entry point                                
    pushl %edx          

    # push IRET context to kernel stack
    iret

/*
cli;
mov $0x2B, %ax
mov %ax, %ds
pushl $0x2B

pushfl
popl %edx
orl $0x200, %edx
pushl %edx

pushl $0x23
pushl %0
*/