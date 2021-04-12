.data
# Assembly linkage for interrupt handlers (RTC, system call, keyboard, etc.)
# used to save registers before calling handler

.globl context_switch

context_switch:
    # get parameters (esp, entry_point)
    movl 4(%esp), %ecx
    movl 8(%esp), %edx

    # push USER_DS                    
    xorl %eax, %eax               
    movw 0x23, %ax                 
    pushl %eax                     
                                    
    # push ESP                      
    pushl %edx                        
                                    
    # push EFLAGS                   
    pushfl                          
                                    
    # push CS - 0x2B (43)           
    xorl %eax, %eax               
    movw 0x2B, %ax                 
    pushl %eax                     

    # push entry point                                
    pushl %ecx          

    # push IRET context to kernel stack
    iret
