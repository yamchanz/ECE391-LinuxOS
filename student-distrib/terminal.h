#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "keyboard.h"
#include "paging.h"

// cursor related macros (VGA registers)
#define CURSOR_LOW      0x0F
#define CURSOR_HIGH     0x0E
#define VGA_CTRL        0x3D4
#define VGA_DATA        0x3D5
// Terminal related macro and data structure
#define BUF_SIZE        128
#define BUF_END_CHAR    0x10    // ascii line limiter

#define TERMINAL_COUNT  3
#define PROCESS_COUNT   6

typedef struct __attribute__((packed)) terminal {
    uint32_t screen_x;
    uint32_t screen_y;

    char* video_mem;

    uint8_t buffer[BUF_SIZE];
    uint32_t buffer_idx;

    int process_ct;
    int running_process;
    int shell_flag;
} terminal_t;

// Terminal array
terminal_t t[TERMINAL_COUNT];
// active terminal index
int32_t t_run;
int32_t t_cur;
int32_t t_visible;

// Clear the screen and put the cursor at the top
void terminal_reset(void);
// Initialize terminal
void terminal_init(void);
// Clears terminal buffer
void clear_buffer(void);

// Open the terminal and display it
int32_t terminal_open(const uint8_t *filename);
// Close terminal and make it available for later
int32_t terminal_close(int32_t fd);

// Write the content of the line buffer to the given buffer
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
// Read the content of the given buffer and write on the terminal
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

// Delete the first line and move everything up a line
void scroll_up(void);
// Update the cursor position
void update_cursor(void);

#endif
