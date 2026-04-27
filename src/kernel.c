#include <stdint.h>
#include "../include/strings.h"
#include "../include/kio.h"
#include "../include/arrays.h"

char buf[256];
int running = 1;

void shell_exec(const char* s) {   // how many rewrites has this function had again?
    // tokenisation
    int count = 0;
    int max_tokens = 16;
    char* tokens[max_tokens];
    char* p = s;

    while (*p != '\0' && count < max_tokens) {
        while (*p == ' ') p++;

        if (*p == '\0') break;

        tokens[count] = p;
        count++;

        while (*p != '\0' && *p != ' ') p++;

        if (*p == '\0') break;

        *p = '\0';
        p++;
    }

    // execution (cuz who tf needs parsing)
    char* command = tokens[0];
    if (equal(command, "clear")) {
        clear(WHITE, BLACK);
    } else if (equal(command, "exit")) {
        running = 0;
    } else if (equal(command, "echo")) {
        if (count < 2) {
            print("Missing argument in echo\n", LIGHT_RED, BLACK);
            return;
        }
        print(tokens[1], WHITE, BLACK);
        putchar('\n', WHITE, BLACK);
    } else if (equal(command, "read")) {
        char read[256];
        if (count > 1) {
            input(read, 256, tokens[1], WHITE, BLACK, 0);
        } else {
            input(read, 256, "", WHITE, BLACK, 0);
        }
        print(read, WHITE, BLACK);
    } else {
        print("Not a valid command\n", LIGHT_RED, BLACK);
        return;
    }
}

void c_start() {
    clear(WHITE, BLACK);

    print("Hello from C kernel!\n", WHITE, BLACK);   // pretend this comment is funny  <-- really funny
    print("Everything works!\n", WHITE, BLACK);
    print("Finally :D\n", WHITE, BLACK);
    print("Once upon a time there was this guy called Bob. Bob was a little bit of a cheeky person but nonetheless he is still very [insert cool adjective]!\n", WHITE, BLACK);
    input(buf, 256, "Enter something: ", WHITE, BLACK, 0);
    print(buf, WHITE, BLACK);
    print("Thanks for your input!\n", WHITE, BLACK);
    while (running) {
        input(buf, 256, "$ ", WHITE, BLACK, 1);
        if (buf[0] != '\0') {       // The user didn't type anything
            shell_exec(buf);
        }
    }
    
    while(1){}
}