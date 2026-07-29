#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("you done fucked up");
        exit(1); 
    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL) {
        perror("");
        exit(2);
    }

    char buffer[4096];

    while (fgets(buffer, 4096, file)) {
        fputs(buffer, stdout);
    }


    fclose(file);

    return 0;
}

