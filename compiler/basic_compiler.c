#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../stringview.c"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    FILE *src = fopen(argv[1], "r");
    FILE *out = fopen("/tmp/transpiler_temp.c", "w");
    if (!src || !out) {
        perror("error opening source file");
        if (src) fclose(src);
        fclose(out);
        exit(1);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), src)) {
        StringView sv = { .text = buffer };
        sv_init_count(&sv);
        sv_trim(&sv);
        if (sv.count == 0) continue;
        while (sv.count > 0) {
            StringView token = sv_tok(&sv, ' ');
            if (token.count == 0) break;

            printf(STRFORMAT"\n", STRARGS(token));
        }
    }

    fclose(src);
    fclose(out);
    remove(argv[1]);
    return 0;
}
