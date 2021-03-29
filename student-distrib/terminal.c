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
 * function implementation copied from https://wiki.osdev.org/Text_Mode_Cursor 
 * description from https://stackoverflow.com/questions/25321608/moving-text-mode-cursor-not-working */
void update_cursor(void) {
    uint16_t position = t.screen_y * NUM_COLS + t.screen_x; // hold two 8 bits -> 16 bits
    outb(CURSOR_LOW, VGA_CTRL);
    outb((uint8_t)(position & 0xFF), VGA_DATA);
    outb(CURSOR_HIGH, VGA_CTRL);
    outb((uint8_t)((position >> 8) & 0xFF), VGA_DATA);
}

/* void read_terminal(void);
 * Inputs: void
 * Outputs: out_buffer -- buffer to store the line buffer copy
 * Return Value: none
 * Function: read (copy) the content of the line buffer to the given buffer */
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

/* void write_terminal(void);
 * Inputs: in_buffer -- buffer to be written on the terminal
 * Return Value: none
 * Function: write the content of the given buffer on the terminal */
void write_terminal(void* in_buffer) {
    int32_t i;

    for (i = 0; i < BUF_SIZE && i < strlen(in_buffer); ++i) {
        if (i == NUM_COLS) 
            putc('\n');
        putc(((uint8_t *)in_buffer)[i]);
    }
    putc('\n');
    clear_buffer();
}

/* void scroll_up(void);
 * Inputs: void
 * Return Value: none
 * Function: delete the first line and move everything up a line */
void scroll_up(void) {
    int32_t i;

    memmove(t.video_mem, t.video_mem + (NUM_COLS << 1), (NUM_COLS * (NUM_ROWS - 1)) << 1);
    for (i = (NUM_ROWS - 1) * NUM_COLS; i < NUM_ROWS * NUM_COLS; ++i) {
        *(uint8_t *)(t.video_mem + (i << 1))     = ' ';
        *(uint8_t *)(t.video_mem + (i << 1) + 1) = ATTRIB;
    }
}
