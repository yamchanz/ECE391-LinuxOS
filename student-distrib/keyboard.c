#include "keyboard.h"

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

/* keyboard_init
    DESCRIPTION: initializes keyboard by setting the default flag and enabling on the PIC
    INPUTS: none
    OUTPUTS: enables KEYBOARD_IRQ(1) on the PIC, writes to the RTC registers
    RETURN VALUE: none
    SIDE EFFECTS: none
*/
void keyboard_init(void) {
<<<<<<< HEAD
    int i;
=======
>>>>>>> 553156ccfa6732da100f10eee5cae963d250a1e0
    keyboard_flag = 0x00;
    for (i = 0; i < KEY_BUF_SIZE; ++i) 
        keyboard_buf[i] = 0;
    keyboard_buf_idx = 0;
    enable_irq(KEYBOARD_IRQ);
}

/* keyboard_handler
    DESCRIPTION: installs the interrupt handler for the RTC
    INPUTS: none
    OUTPUTS: change keyboard flag or echo key based on the input
    RETURN VALUE: none
    SIDE EFFECTS: modifies terminal
*/
void keyboard_handler(void) {
    uint8_t scan_code, key_ascii;   // store scan code and translation to ascii

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

        // not implemented yet
        case ENTER_PRS:
            send_eoi(KEYBOARD_IRQ);
            return;

        case CAPS_LOCK_REL:
        case ENTER_REL:
            send_eoi(KEYBOARD_IRQ);
            return;
<<<<<<< HEAD

        case BACKSPACE_PRS:
            if (keyboard_buf_idx) {
                keyboard_buf[keyboard_buf_idx--] = 0;
                putc('\b');
            }
            send_eoi(KEYBOARD_IRQ);
            return;
            
=======
        
        // if not special, get the ascii character based on the flag status
>>>>>>> 553156ccfa6732da100f10eee5cae963d250a1e0
        default: 
            key_ascii = scan_code_to_ascii[(keyboard_flag >> 6) & 0x03][scan_code];
    }

<<<<<<< HEAD
    // check for ctrl + L
    if ((keyboard_flag & CTRL_MASK) && key_ascii == 'L') {
        // clear the screen and put the cursor at the top
=======
    // echo the ascii character
    if (key_ascii) {
        putc(key_ascii);
        send_eoi(KEYBOARD_IRQ);
>>>>>>> 553156ccfa6732da100f10eee5cae963d250a1e0
        return;
    } 

    // print unless reached the buffer size limit
    if (key_ascii && keyboard_buf_idx != KEY_BUF_SIZE - 1) {
        keyboard_buf[keyboard_buf_idx++] = key_ascii;
        putc(key_ascii);
    }
<<<<<<< HEAD
=======
    // never reaches here (implement return error later)
    send_eoi(KEYBOARD_IRQ);
    return;
>>>>>>> 553156ccfa6732da100f10eee5cae963d250a1e0
}
