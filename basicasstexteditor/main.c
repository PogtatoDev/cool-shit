#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <ctype.h>

void raw_mode(struct termios *raw) {
    raw->c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, raw);
}


int main() {
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);
    struct termios raw = original;

    raw_mode(&raw);

    char c;

    while (read(STDIN_FILENO, &c, 1) && c != 'q') {
        if (iscntrl(c)) {
            printf("%d\n", c);
        } else {
            printf("%d ('%c')\n", c, c);
        }
    };

    // reset terminal attributes to their origin state
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);

    return 0;
}
