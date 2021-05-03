#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "i8259.h"
#include "terminal.h"
// #include "paging.h"

// IRQ and port
#define KEYBOARD_IRQ    1
#define KEYBOARD_PORT   0x60

// macros for the special input
#define REL_MASK        0x80
#define CAPS_LOCK_PRS   0x3A
#define L_SHIFT_PRS     0x2A
#define L_SHIFT_REL     (L_SHIFT_PRS | REL_MASK)
#define R_SHIFT_PRS     0x36
#define R_SHIFT_REL     (R_SHIFT_PRS | REL_MASK)
#define ALT_PRS         0x38
#define ALT_REL         (ALT_PRS | REL_MASK)
#define CTRL_PRS        0x1D
#define CTRL_REL        (CTRL_PRS | REL_MASK)
#define ENTER_PRS       0x1C
#define BACKSPACE_PRS   0x0E
#define terminal_1      0
#define terminal_2      1
#define terminal_3      2

// function keys for changing terminals
#define F1              0x3B
#define F2              0x3C
#define F3              0x3D

extern uint8_t get_enter_flag(void);
extern void release_enter(void);
// initializes keyboard by setting the default flag and enabling on the PIC
extern void keyboard_init(void);
// installs the interrupt handler for the RTC
extern void keyboard_handler(void);

#endif
