#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"

// cursor related macros (VGA registers)
#define CURSOR_LOW      0x0F
#define CURSOR_HIGH     0x0E
#define VGA_CTRL        0x3D4
#define VGA_DATA        0x3D5
// Terminal related macro and data structure
#define BUF_SIZE        128
#define BUF_END_CHAR    0x10    // ascii line limiter
typedef struct terminal {
    uint32_t screen_x;
    uint32_t screen_y;
    
    char* video_mem;

    uint8_t buffer[BUF_SIZE];
    uint32_t buffer_idx;
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
