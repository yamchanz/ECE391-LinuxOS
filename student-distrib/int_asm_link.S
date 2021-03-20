#define ASM     1
#include "int_asm_link.h"

.globl sys_call_handler


; sys_call_handler:
;     pushl %eax
;     call sys_call_handler
;     popl %eax