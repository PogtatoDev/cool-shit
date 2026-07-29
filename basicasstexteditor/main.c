#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#define ctrl(key) (key & 0x1f)

struct termios original;

void clear();

void crash(char *msg) {
    clear();

    perror(msg);
    exit(1);
}

void reset_terminal() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original) == -1) crash("tcsetattr");
}

void raw_mode(struct termios *raw) {
    raw->c_lflag &= ~(ECHO | IEXTEN | ICANON | ISIG);
    raw->c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw->c_oflag &= ~(OPOST);
    raw->c_cflag |= (CS8);
    raw->c_cc[VMIN] = 0;
    raw->c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, raw) == -1) crash("tcsetattr");
}

void draw_line_numbers() {
    for (int y = 0; y < 24; y++) {
        printf("%d  \r\n", y);
    }
}


char read_key() {
    char c;
    int returnval;
    while ((returnval = read(STDIN_FILENO, &c, 1)) != 1) {
        if (returnval == -1) crash("read");
    }
    return c;
}

void process_key(char key) {
    switch (key) {
        case ctrl('q'):
            clear();
            exit(0);
            break;
    }
}

void clear() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[1;4H", 3);

    draw_line_numbers();
    write(STDOUT_FILENO, "\x1b[1;4H", 3);

}

int main() {
    if (tcgetattr(STDIN_FILENO, &original) == -1) crash("tcgetattr");
    struct termios raw = original;
    atexit(reset_terminal);

    raw_mode(&raw);

    while (1) {
        char c = read_key();

        clear();
        process_key(c);
    }

    return 0;
}
