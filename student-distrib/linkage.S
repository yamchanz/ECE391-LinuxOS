#define ASM     1
#include "x86_desc.h"

.GLOBL  Divide_Error

Divde_Error:
pushal
call Divide_Error_Exception
popal
iret

Keyboard:

