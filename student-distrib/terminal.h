#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"

// Terminal related macro and data structure
#define BUF_SIZE        128
#define BUF_END_CHAR    0x10    // ascii line limiter
typedef struct terminal {
    int screen_x;
    int screen_y;
    
    char* video_mem;

    uint8_t buffer[BUF_SIZE];
    int buffer_idx;
} terminal_t;

// Terminal variable
terminal_t t;

// Clear the screen and put the cursor at the top
void reset_terminal(void);
// Initialize terminal
void init_terminal(void);
// Update the cursor position
void update_cursor(void);
// Clears terminal buffer
void clear_buffer(void);
// write the content of the line buffer to the given buffer
void read_terminal(void *out_buffer);
// read the content of the given buffer and write on the terminal
void write_terminal(void *in_buffer);
// delete the first line and move everything up a line
void scroll_up(void);

#endif
