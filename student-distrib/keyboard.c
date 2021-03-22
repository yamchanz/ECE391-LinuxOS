#include "keyboard.h"

// local functions

// format copied from https://stackoverflow.com/questions/61124564/convert-scancodes-to-ascii
// 0: none 1: caps_lock 2: shift 3: caps_lock && shift
uint8_t scan_code_to_ascii[4][128] = {{
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 0, 
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0,  /* Alt */
    0,  /* Space bar */
    0,  /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 59 - F1 ~ F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0, 0, 0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
}, {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0, 
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*',
    0,  /* Alt */
    0,  /* Space bar */
    0,  /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 59 - F1 ~ F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0, 0, 0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
}, {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 0,
    'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 
    0,  /* Alt */
    0,  /* Space bar */
    0,  /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 59 - F1 ~ F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0, 0, 0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
}, {
    
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 0,
    'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '{', '}', 0, 0, 
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~', 0, '|', 
    'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', 0, '*', 
    0,  /* Alt */
    0,  /* Space bar */
    0,  /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 59 - F1 ~ F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0, 0, 0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
}};

// MSB to LSB: caps_lock, shift, alt, ctrl, enter, nul, nul, nul
static uint8_t keyboard_flag;

void keyboard_init(void) {

    keyboard_flag = 0x00;
    enable_irq(KEYBOARD_IRQ);
}

void keyboard_handler(void) {
    uint8_t scan_code, key_ascii;

    scan_code = inb(KEYBOARD_PORT);
    // check special cases
    switch(scan_code) {
        case CAPS_LOCK_PRS:
            keyboard_flag = (keyboard_flag & CAPS_LOCK_MASK) ? 
                (keyboard_flag & ~CAPS_LOCK_MASK) : (keyboard_flag | CAPS_LOCK_MASK);
            send_eoi(KEYBOARD_IRQ);
            return;

        case L_SHIFT_PRS:
        case R_SHIFT_PRS:
            keyboard_flag |= SHIFT_MASK;
            send_eoi(KEYBOARD_IRQ);
            return;

        case L_SHIFT_REL:
        case R_SHIFT_REL:
            keyboard_flag &= ~SHIFT_MASK;
            send_eoi(KEYBOARD_IRQ);
            return;

        case ALT_PRS:
            keyboard_flag |= ALT_MASK;
            send_eoi(KEYBOARD_IRQ);
            return;
        case ALT_REL:
            keyboard_flag &= ~ALT_MASK;
            send_eoi(KEYBOARD_IRQ);
            return;

        case CTRL_PRS:
            keyboard_flag |= CTRL_MASK;
            send_eoi(KEYBOARD_IRQ);
            return;
        case CTRL_REL:
            keyboard_flag &= ~CTRL_MASK;
            send_eoi(KEYBOARD_IRQ);
            return;

        case ENTER_PRS:
            // putc('\n');
            send_eoi(KEYBOARD_IRQ);
            return;

        case CAPS_LOCK_REL:
        case ENTER_REL:
            send_eoi(KEYBOARD_IRQ);
            return;
            
        default: 
            key_ascii = scan_code_to_ascii[(keyboard_flag >> 6) & 0x03][scan_code];
    }

    // print otherwise
    if (key_ascii) {
        putc(key_ascii);
        send_eoi(KEYBOARD_IRQ);
        return;
    }
    // never reaches here
    send_eoi(KEYBOARD_IRQ);
    return;
}
