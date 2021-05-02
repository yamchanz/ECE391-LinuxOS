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

// caps_lock, shift, alt, ctrl, enter for each terminal flags
static uint8_t caps_lock_flag;
static uint8_t shift_flag;
static uint8_t alt_flag;
static uint8_t ctrl_flag;
static uint8_t enter_flag[3];

/*  enter_pressed
 *  DESCRIPTION: return if enter is pressed
 *  INPUTS: none
 *  OUTPUTS: 1 (true) if pressed 0 if not
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none */
uint8_t get_enter_flag(void) {
    return enter_flag[t_visible];
}

void release_enter(void) {
    enter_flag[t_visible] = 0;
}

/*  keyboard_init
 *  DESCRIPTION: initializes keyboard by setting the default flag and enabling on the PIC
 *  INPUTS: none
 *  OUTPUTS: enables KEYBOARD_IRQ(1) on the PIC, writes to the RTC registers
 *  RETURN VALUE: none
 *  SIDE EFFECTS: none */
void keyboard_init(void) {
    int32_t i;
    caps_lock_flag = 0;
    shift_flag = 0;
    alt_flag = 0;
    ctrl_flag = 0;
    for (i = 0; i < TERMINAL_COUNT; ++i)
        enter_flag[t_visible] = 0;
    enable_irq(KEYBOARD_IRQ);
}

/*  keyboard_handler
 *  DESCRIPTION: installs the interrupt handler for the RTC
 *  INPUTS: none
 *  OUTPUTS: change keyboard flag or echo key based on the input
 *  RETURN VALUE: none
 *  SIDE EFFECTS: modifies terminal */
void keyboard_handler(void) {
    uint8_t scan_code, key_ascii, which_keys;   // store scan code and translation to ascii

    send_eoi(KEYBOARD_IRQ);
    scan_code = inb(KEYBOARD_PORT);
    // check special cases
    switch(scan_code) {
        case CAPS_LOCK_PRS:
            if (caps_lock_flag)
                caps_lock_flag = 0;
            else 
                caps_lock_flag = 1;
            return;

        case L_SHIFT_PRS:
        case R_SHIFT_PRS:
            shift_flag = 1;
            return;

        case L_SHIFT_REL:
        case R_SHIFT_REL:
            shift_flag = 0;
            return;

        case ALT_PRS:
            alt_flag = 1;
            return;

        case ALT_REL:
            alt_flag = 0;
            return;

        case CTRL_PRS:
            ctrl_flag = 1;
            return;

        case CTRL_REL:
            ctrl_flag = 0;
            return;

        case ENTER_PRS:
            // from LSB, t[0], t[1], t[2]
            enter_flag[t_visible] = 1;
            putc('\n');
            return;

        case BACKSPACE_PRS:
        if (t[t_visible].buffer_idx) {
                t[t_visible].buffer[--t[t_visible].buffer_idx] = '\0';  // buffer limiter
                putc('\b');
            }
            return;

        default: ;
    }

    // if not special, get the ascii character based on the flag status
    // 1st bit: caps lock flag, 2nd bit: shift flag after shift
    which_keys = (caps_lock_flag << 1) + shift_flag;
    key_ascii = scan_code_to_ascii[which_keys][scan_code];

    // check for terminal switch
    if (alt_flag) {
        switch (scan_code) {
            // switch to terminal 0
            case F1:
                switch_display(0);
                break;
            // switch to terminal 1
            case F2:
                switch_display(1);
                if(t[1].shell_flag == -1) {
                    pcb_t* pcb = get_pcb(1);

                    asm volatile(
                        "movl %%esp, %0     \n"
                        "movl %%ebp, %1     \n"
                        :"=r"(pcb->cur_esp), "=r"(pcb->cur_ebp) // output
                        : // input
                    );
                    t[t_visible].running_process = 1;
                    execute((uint8_t*)"shell");
                }
                break;
            // switch to terminal 2
            case F3:
                switch_display(2);
                if(t[2].shell_flag == -1) {
                    pcb_t* pcb = get_pcb(2);

                    asm volatile(
                        "movl %%esp, %0     \n"
                        "movl %%ebp, %1     \n"
                        :"=r"(pcb->cur_esp), "=r"(pcb->cur_ebp) // output
                        : // input
                    );
                    t[t_visible].running_process = 2;
                    execute((uint8_t*)"shell");
                }
                break;
            default: ;
        }
        return;
    }
    // check for CTRL-L
    if (ctrl_flag && (key_ascii == 'L' || key_ascii == 'l')) {
        terminal_reset();
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
