#include "terminal.h"

/* void clear_buffer(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears terminal buffer */
void clear_buffer(void) {
    int32_t i;
    for (i = 0; i < BUF_SIZE; ++i) 
        t.buffer[i] = ' ';
    t.buffer_idx = 0;
}

/* void init_terminal(void);
 * Inputs: void
 * Return Value: none
 * Function: Clear the screen and put the cursor at the top */
void init_terminal(void) {
    t.video_mem = (char *)VIDEO;
    reset_terminal();
}

/* void reset_terminal(void);
 * Inputs: void
 * Return Value: none
 * Function: Clear the screen and put the cursor at the top */
void reset_terminal(void) {
    t.screen_x = 0, t.screen_y = 0;
    clear();
    clear_buffer();
    update_cursor();
}

/* void update_cursor(void);
 * Inputs: void
 * Return Value: none
 * Function: update the cursor position
 * function implementation copied from https://wiki.osdev.org/Text_Mode_Cursor */
void update_cursor(void) {
    outb(0x0F, 0x3D4);
    outb((uint8_t)(t.screen_x & 0xFF), 0x3D5);
    outb(0x0E, 0x3D4);
    outb((uint8_t)(t.screen_y & 0xFF), 0x3D5);
}

void read_terminal(void* out_buffer) {
    int32_t i;

    if (out_buffer) {
        for (i = 0; i < BUF_SIZE; ++i) {
        // cast to uint8_t and copy
        ((uint8_t *)out_buffer)[i] = t.buffer[i];
        }
    }
    clear_buffer();
}
