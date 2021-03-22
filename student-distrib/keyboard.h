#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "i8259.h"

// IRQ and port
#define KEYBOARD_IRQ    1
#define KEYBOARD_PORT   0x60

// macros for the special input
#define REL_MASK        0x80
#define CAPS_LOCK_PRS   0x3A
#define CAPS_LOCK_REL   (CAPS_LOCK_PRS | REL_MASK)
#define L_SHIFT_PRS     0x2A
#define L_SHIFT_REL     (L_SHIFT_PRS | REL_MASK)
#define R_SHIFT_PRS     0x36
#define R_SHIFT_REL     (R_SHIFT_PRS | REL_MASK)
#define ALT_PRS         0x38
#define ALT_REL         (ALT_PRS | REL_MASK)
#define CTRL_PRS        0x1D
#define CTRL_REL        (CTRL_PRS | REL_MASK)
#define ENTER_PRS       0x1C
#define ENTER_REL       (ENTER_PRS | REL_MASK)
#define BACKSPACE_PRS   0x0E

// MSB to LSB: caps_lock, shift, alt, ctrl, enter, nul, nul, nul; 1: on, 0: off
#define CAPS_LOCK_MASK  0x80
#define SHIFT_MASK      0x40
#define ALT_MASK        0x20
#define CTRL_MASK       0x10
#define ENTER_MASK      0x08

// initializes keyboard by setting the default flag and enabling on the PIC
extern void keyboard_init(void);
// installs the interrupt handler for the RTC
extern void keyboard_handler(void);


#endif
