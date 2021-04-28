#include "terminal.h"

/* void clear_buffer(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears terminal buffer */
void clear_buffer(int32_t tid) {
    int32_t i;
    for (i = 0; i < BUF_SIZE; ++i) 
        t[tid].buffer[i] = '\0';
    t[tid].buffer_idx = 0;
}

/* void terminal_init(void);
 * Inputs: void
 * Return Value: none
 * Function: Clear the screen and put the cursor at the top */
void terminal_init(void) {
    int32_t i;
    for (i = 0; i < TERMINAL_COUNT; ++i) {
        t[i].video_mem = (char *)(VIDEO + _4_KB * i);
        terminal_reset(i);
    }
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
    int32_t i;
    for (i = 0; i < TERMINAL_COUNT; ++i)
        clear_buffer(i);
    return -1;
}

/* void terminal_reset(void);
 * Inputs: void
 * Return Value: none
 * Function: Clear the screen and put the cursor at the top */
void terminal_reset(int32_t tid) {
    t[tid].screen_x = 0, t[tid].screen_y = 0;
    clear();
    clear_buffer(tid);
    update_cursor(tid);
}

/* void update_cursor(void);
 * Inputs: void
 * Return Value: none
 * Function: update the cursor position
 * function implementation copied from https://wiki.osdev.org/Text_Mode_Cursor 
 * description from https://stackoverflow.com/questions/25321608/moving-text-mode-cursor-not-working */
void update_cursor(int32_t tid) {
    uint16_t position = t[tid].screen_y * NUM_COLS + t[tid].screen_x; // hold two 8 bits -> 16 bits
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

    clear_buffer(t_run);
    sti();
    while(t[t_run].buffer_idx < BUF_SIZE - 1 && !get_enter_flag());
    cli();
    size = nbytes > t[t_run].buffer_idx ? t[t_run].buffer_idx : nbytes;
    for (i = 0; i < size; ++i) {
        ((int8_t*)buf)[i] = t[t_run].buffer[i];
    }
    release_enter();
    clear_buffer(t_run);
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
        putc(((uint8_t *)buf)[i]);

    return nbytes;
}

/* void scroll_up(void);
 * Inputs: void
 * Return Value: none
 * Function: delete the first line and move everything up a line */
void scroll_up(int32_t tid) {
    int32_t i;

    memmove(t[tid].video_mem, t[tid].video_mem + (NUM_COLS << 1), (NUM_COLS * (NUM_ROWS - 1)) << 1);
    for (i = (NUM_ROWS - 1) * NUM_COLS; i < NUM_ROWS * NUM_COLS; ++i) {
        *(uint8_t *)(t[tid].video_mem + (i << 1))     = ' ';
        *(uint8_t *)(t[tid].video_mem + (i << 1) + 1) = ATTRIB;
    }
}

// int32_t get_parent_pid(int32_t tid) {
//     int32_t i;
//     for (i = pid - 1; i >= 0; --i) {
//         if (t[tid].pid[i])
//             return i;
//     }
//     // return itself when there isn't a parent process running
//     return pid;
// }

void terminal_switch(int32_t tid) {
    int32_t prev_t_run = t_run;
    if (tid == prev_t_run) 
        return;
    
}
