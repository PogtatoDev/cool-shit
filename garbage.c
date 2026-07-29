#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int main() {
    while (1) {
        char c = rand() % 128;
        if (!iscntrl(c)) printf("%c", c);
    }

    return 0;
}
