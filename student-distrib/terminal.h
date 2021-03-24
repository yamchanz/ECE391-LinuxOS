#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"

// Terminal related macro and data structure
#define BUF_SIZE    128
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
void read_terminal(void *out_buffer);

#endif
