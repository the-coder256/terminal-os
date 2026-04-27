#ifndef OS_IO_H_
#define OS_IO_H_ 0

#include <stdint.h>
#include "../include/strings.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile uint16_t* VGA = (volatile uint16_t*)0xb8000;   // very low level i think

int cursor_x = 0;
int cursor_y = 0;

int shift_pressed = 0;
uint8_t last_scancode = 0;

// VGA colors
enum VGAColor {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GREY = 7,
    DARK_GREY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_BROWN = 14,
    WHITE = 15
};

static char scancode_map[128] = {   // Edit this to match MY keyboard layout (no ill do it other way)
    0,  27,'1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,  '*',0,' ',
};

static char scancode_map_shift[128] = {   // Edit this too
    0,  27,'!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?',0,  '*',0,' ',
};

static inline uint8_t inb(uint16_t port) {   // Ok I have no idea what this is doing
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint8_t read_scancode() {  // Use read_key
    return inb(0x60);      // Read from PS/2 data port (whatever that is)
}

char read_key() {
    uint8_t sc = inb(0x60);

    // Prevent repeats
    if (sc == last_scancode) return 0;
    last_scancode = sc;

    if (sc >= 128) return 0;
    return sc;
    return scancode_map[sc];
}

char scancode_to_ascii(uint8_t sc) {
    if (sc == 0) return 0;
    return scancode_map[sc];
}

void update_shift(uint8_t sc) {
    if (sc == 0x2A || sc == 0x36) shift_pressed = 1;  // press
    else if (sc == 0xAA || sc == 0xB6) shift_pressed = 0;  // release
}

void process_key(char c) {
    if (!c) return;
    if (c == '\b') { // backspace
        if (cursor_x > 0) {
            cursor_x--;
            VGA[cursor_y*VGA_WIDTH + cursor_x] = (BLACK<<12) | ' ';
        }
    } else if (c == '\n') { // enter
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) scroll();
    } else { // normal character
        VGA[cursor_y*VGA_WIDTH + cursor_x] = (WHITE<<8)|c;
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) { cursor_x = 0; cursor_y++; if (cursor_y >= VGA_HEIGHT) scroll(); }
    }
}

void clear(uint8_t fg, uint8_t bg) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA[y * VGA_WIDTH + x] = ((bg << 12) | (fg << 8)) | ' ';
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void scroll() {
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA[(y-1)*VGA_WIDTH + x] = VGA[y*VGA_WIDTH + x];
        }
    }
    // Clear last line
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA[(VGA_HEIGHT-1)*VGA_WIDTH + x] = (BLACK << 12) | ' ';
    }
    if (cursor_y > 0) cursor_y--;
}

void backspace(uint8_t fg, uint8_t bg) {
    if (cursor_x > 0) {
        cursor_x--;
        putchar('\0', fg, bg);
        cursor_x--;
    } else if (cursor_x == 0 && cursor_y > 0) {
        cursor_x = VGA_WIDTH - 1;
        cursor_y--;
        putchar('\0', fg, bg);
        cursor_x--;
    }
}

void putchar(char c, uint8_t fg, uint8_t bg) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) scroll();
        return;
    }
    VGA[cursor_y * VGA_WIDTH + cursor_x] = ((bg << 12) | (fg << 8)) | c;
    cursor_x++;
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) scroll();
    }
}

void print(const char* s, uint8_t fg, uint8_t bg){
    for(int i = 0; s[i]; i++) {
        putchar(s[i], fg, bg);
    }
}

void input(char* buf, int buf_size, const char* s, uint8_t fg, uint8_t bg, int ignore_newline) {
    print(s, fg, bg);
    int base_cursor_x = cursor_x;
    int base_cursor_y = cursor_y;
    clrbuf(buf);
    char string[256];
    for (int i = 0; i < 256; i++) {
        string[i] = 0;
    }
    int str_pointer = 0;
    while(1) {
        uint8_t sc = read_key();
        char c = scancode_to_ascii(sc);
        if (c == '\b') {
            if ((cursor_y * VGA_WIDTH + cursor_x) > (base_cursor_y * VGA_WIDTH + base_cursor_x)) {
                backspace(fg, bg);
                str_pointer--;
                string[str_pointer] = '\0';
                continue;
            }
            continue;
        }
        if (c != 0) {
            putchar(c, fg, bg);
            if (ignore_newline) {
                if (c != '\n') {
                    string[str_pointer] = c;
                    str_pointer++;
                }
            } else {
                string[str_pointer] = c;
                str_pointer++;
            }
        }
        if (c == '\n') {
            for (int i = 0; i < buf_size; i++) {
                buf[i] = string[i];
            }
            buf[255] = '\0';
            break;
        }
    }
}

#endif