#ifndef STRINGS_H
#define STRINGS_H 0

int len(const char* s) {   /* Returns the length of the inputted string. */
    int length = 0;
    while (s[length] != '\0') {
        length++;
    }
    return length;
}

char* concat(char* a, const char* b) {   /* Concatenates two strings together and returns the result. */
    char* c = a;
    while (*a) {
        a++;
    }
    while (*b) {
        *a = *b;
        a++;
        b++;
    }
    *a = '\0';
    return c;
}

char* copy(char* dest, const char* src) {   /* Copies the source to the destination. Returns pointer to new destination. */
    char* start = dest;
    while (src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return start;
}

void clrbuf(char* buf) {   /* Empties an array */
    for (int i = 0; i < sizeof(buf) / sizeof(char); i++) {
        buf[i] = 0;
    }
}

int equal(const char* s1, const char* s2) {   /* Checks if two strings are equal. */
    if (len(s1) != len(s2)) {
        return 0;
    }

    // From here, we know that s1 and s2 are the same length (hopefully)
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            return 0;
        }
        s1++;
        s2++;
    }

    // No characters were not the same, meaning that they are equal!
    return 1;
}

#endif