#include "keyboard.h"
#include "paging.h"
#include "system_calls.h"

// format copied from https://stackoverflow.com/questions/61124564/convert-scancodes-to-ascii
// 0: none 1: shift 2: caps_lock 3: caps_lock && shift
uint8_t scan_code_to_ascii[4][128] = {{
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0,  /* Alt */
    ' ',  /* Space bar */
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
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
    'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',
    0,  /* Alt */
    ' ',  /* Space bar */
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
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*',
    0,  /* Alt */
    ' ',  /* Space bar */
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

    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
    'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '{', '}', 0, 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~', 0, '|',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', 0, '*',
    0,  /* Alt */
    ' ',  /* Space bar */
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

// MSB to LSB: caps_lock, shift, alt, ctrl, nul, nul, nul, nul
static uint8_t keyboard_flag;
static uint8_t enter_flag;

/*  enter_pressed
 *  DESCRIPTION: return if enter is pressed
 *  INPUTS: none
 *  OUTPUTS: 1 (true) if pressed 0 if not
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none */
uint8_t get_enter_flag(void) {
    return enter_flag & (1 << t_visible);
}

void release_enter(void) {
    enter_flag &= ~(1 << t_visible);
}

/*  keyboard_init
 *  DESCRIPTION: initializes keyboard by setting the default flag and enabling on the PIC
 *  INPUTS: none
 *  OUTPUTS: enables KEYBOARD_IRQ(1) on the PIC, writes to the RTC registers
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none */
void keyboard_init(void) {
    keyboard_flag = 0;
    enter_flag = 0;
    enable_irq(KEYBOARD_IRQ);
}

/*  keyboard_handler
 *  DESCRIPTION: installs the interrupt handler for the RTC
 *  INPUTS: none
 *  OUTPUTS: change keyboard flag or echo key based on the input
 *  RETURN VALUE: none
 *  SIDE EFFECTS: modifies terminal */
void keyboard_handler(void) {

    send_eoi(KEYBOARD_IRQ);
    uint8_t scan_code, key_ascii;   // store scan code and translation to ascii

    scan_code = inb(KEYBOARD_PORT);
    // check special cases
    switch(scan_code) {
        case CAPS_LOCK_PRS:
            keyboard_flag = (keyboard_flag & CAPS_LOCK_MASK) ?
                (keyboard_flag & ~CAPS_LOCK_MASK) : (keyboard_flag | CAPS_LOCK_MASK);
            return;

        case L_SHIFT_PRS:
        case R_SHIFT_PRS:
            keyboard_flag |= SHIFT_MASK;
            return;

        case L_SHIFT_REL:
        case R_SHIFT_REL:
            keyboard_flag &= ~SHIFT_MASK;
            return;

        case ALT_PRS:
            keyboard_flag |= ALT_MASK;
            return;

        case ALT_REL:
            keyboard_flag &= ~ALT_MASK;
            return;

        case CTRL_PRS:
            keyboard_flag |= CTRL_MASK;
            return;

        case CTRL_REL:
            keyboard_flag &= ~CTRL_MASK;
            return;

        case ENTER_PRS:
            // from LSB, t[0], t[1], t[2]
            enter_flag |= 1 << t_visible;
            putc('\n');
            return;

        case BACKSPACE_PRS:
        if (t[t_visible].buffer_idx) {
                t[t_visible].buffer[--t[t_visible].buffer_idx] = BUF_END_CHAR;
                putc('\b');
            }
            return;

        default: ;
    }

    // if not special, get the ascii character based on the flag status
    // 1st bit: caps lock flag, 2nd bit: shift flag after shift
    key_ascii = scan_code_to_ascii[(keyboard_flag >> 6) & 0x03][scan_code];

    // check for CTRL-L
    if (keyboard_flag & CTRL_MASK && (key_ascii == 'L' || key_ascii == 'l')) {
        terminal_reset();
        return;
    }
    // check for terminal switch
    if (keyboard_flag & ALT_MASK) {
        switch (scan_code) {
            // switch to terminal 0
            case F1:
                switch_display(0);
                update_cursor();
                break;
            // switch to terminal 1
            case F2:
                switch_display(1);
                update_cursor();
                // if(t[t_visible].shell_flag == -1)
                //     execute((uint8_t*)"shell");
                break;
            // switch to terminal 2
            case F3:
                switch_display(2);
                update_cursor();
                // if(t[t_visible].shell_flag == -1)
                //     execute((uint8_t*)"shell");
                break;
            default: ;
        }
        return;
    }

    // if not release, update the line buffer and echo the ascii character
    if (key_ascii && scan_code < REL_MASK) {
        // go to the next line if the line gets longer than the buffer
        if (t[t_visible].buffer_idx < BUF_SIZE - 2) {
            t[t_visible].buffer[t[t_visible].buffer_idx++] = key_ascii;
            t[t_visible].buffer[t[t_visible].buffer_idx] = '\0';  // line limiter
        } else if (t[t_visible].buffer_idx == BUF_SIZE - 2) {
            t[t_visible].buffer[t[t_visible].buffer_idx++] = '\n';
            t[t_visible].buffer[t[t_visible].buffer_idx] = '\0';  // line limiter
        } else
            t[t_visible].buffer_idx = 0;
        putc(key_ascii);
    }
    return;
}
