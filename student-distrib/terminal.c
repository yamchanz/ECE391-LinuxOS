#include "terminal.h"

/* void clear_buffer(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears terminal buffer */
void clear_buffer(void) {
    int32_t i;
    for (i = 0; i < BUF_SIZE; ++i)
        t[t_visible].buffer[i] = '\0';
    t[t_visible].buffer_idx = 0;
}

/* void terminal_init(void);
 * Inputs: void
 * Return Value: none
 * Function: Clear the screen and put the cursor at the top */
void terminal_init(void) {
    // t[t_visible].video_mem = (char *)VID_MEM; //xb8000
    // terminal_reset();
    int32_t i;
    for(i = 0; i < TERMINAL_COUNT; ++i) {
        t[i].running_process = -1;
        t[i].shell_flag = -1;
    }
    t[0].video_mem = (char*)(VID_MEM + _4_KB); // xb9000
    t[1].video_mem = (char*)(VID_MEM + _8_KB); // xba000
    t[2].video_mem = (char*)(VID_MEM + 3 * _4_KB); // xbb000
    t_visible = 0;
    switch_display(0);
    terminal_reset();
}

/* int32_t terminal_open;
 * Inputs: filename -- unused
 * Return Value: none
 * Function: Open the terminal and display it */
int32_t terminal_open(const uint8_t *filename) {
    terminal_init();
    return 0;
}

/* int32_t terminal_close;
 * Inputs: fd -- unused
 * Return Value: none
 * Function: close terminal and make it available for later */
int32_t terminal_close(int32_t fd) {
    clear_buffer();
    return -1;
}

/* void terminal_reset(void);
 * Inputs: void
 * Return Value: none
 * Function: Clear the screen and put the cursor at the top */
void terminal_reset(void) {
    t[t_visible].screen_x = 0, t[t_visible].screen_y = 0;
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
    uint16_t position = t[t_cur].screen_y * NUM_COLS + t[t_cur].screen_x; // hold two 8 bits -> 16 bits
    outb(CURSOR_LOW, VGA_CTRL);
    outb((uint8_t)(position & 0xFF), VGA_DATA);
    outb(CURSOR_HIGH, VGA_CTRL);
    outb((uint8_t)((position >> 8) & 0xFF), VGA_DATA);
}

/* int32_t terminal_read;
 * Inputs: fd -- unused
           nbytes -- unused
           buf -- address of the data to be sent
 * Outputs:
 * Return Value: size -- the number of chars in buffer
 * Function: read (copy) the content of the line buffer to the given buffer */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    if (!buf || nbytes < 0) return -1;
    int32_t i, size;

    clear_buffer();
    sti();
    while(t[t_cur].buffer_idx < BUF_SIZE - 1 && !get_enter_flag(t_cur));
    cli();
    size = nbytes > t[t_cur].buffer_idx ? t[t_cur].buffer_idx : nbytes;
    for (i = 0; i < size; ++i) {
        ((int8_t*)buf)[i] = t[t_cur].buffer[i];
    }
    release_enter(t_cur);
    clear_buffer();
    return size;
}

/* int32_t terminal_write;
 * Inputs: fd -- unused
           nbytes -- the number of bytes to write
           buf -- pointer to the data to be read from
 * Return Value: nbytes
 * Function: write the content of the given buffer on the terminal */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    if (buf == NULL || nbytes < 0)
        return -1;

    int32_t i;
    for (i = 0; i < nbytes; ++i)
        putc(((uint8_t *)buf)[i], t_cur);

    return nbytes;
}

/* void scroll_up(void);
 * Inputs: void
 * Return Value: none
 * Function: delete the first line and move everything up a line */
void scroll_up(void) {
    int32_t i;

    memmove(t[t_cur].video_mem, t[t_cur].video_mem + (NUM_COLS << 1), (NUM_COLS * (NUM_ROWS - 1)) << 1);
    for (i = (NUM_ROWS - 1) * NUM_COLS; i < NUM_ROWS * NUM_COLS; ++i) {
        *(uint8_t *)(t[t_cur].video_mem + (i << 1))     = ' ';
        *(uint8_t *)(t[t_cur].video_mem + (i << 1) + 1) = ATTRIB;
    }
}
