#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#define NOTES_DIR "/home/seth/.notes/"
#define NOTE_FILENAME_SIZE 128
#define NOTE_DATA_SIZE 2048

void malloc_error_check(void *ptr) {
    if (ptr == NULL) {
        fputs("memory allocation failed\n", stderr);
        exit(-1);
    }
}

void get_processed_name(char *src, char *dest) {
    int i;

    for (i = 0; i < strlen(src); i++) {
        if (src[i] == ' ') dest[i] = '-';
        else dest[i] = src[i];
    }

    dest[i] = '\0';
    dest[strcspn(dest, "\n")] = 0;
}

void remove_substr(char string[], char substr[]) {
    int i = 0;

    int string_length = strlen(string);
    int substr_length = strlen(substr);

    while (i < string_length) {
        if (strstr(&string[i], substr) == &string[i]) {
            string_length -= substr_length;
            for (int j = i; j < string_length; j++)
                string[j] = string[j + substr_length];
        }
        else i++;
    }
    string[i] = '\0';
} // function from https://github.com/portfoliocourses/c-example-code/blob/main/delete_substring.c

void new_note(char *name, char *data) {
    struct stat st = {0};

    if (stat(NOTES_DIR, &st) == -1) {
        mkdir(NOTES_DIR, 0700);
    }

    char *processed_name = (char *)malloc(strlen(name) + 1);
    malloc_error_check(processed_name);

    get_processed_name(name, processed_name);

    char *filename = (char *)malloc(strlen(NOTES_DIR) + strlen(processed_name) + sizeof(".note") + 1);
    malloc_error_check(filename);

    strcpy(filename, NOTES_DIR);
    strcat(filename, processed_name);
    strcat(filename, ".note");

    FILE *note = fopen(filename, "w");
    if (note == NULL) {
        fputs("failed to read file\n", stderr);
        free(filename);
        free(processed_name);
        exit(2);
    }
    fprintf(note, "%s", data);
    fclose(note);

    printf("created file with filename: %s\n", processed_name);

    free(filename);
    free(processed_name);
}

void read_note(char *name) {
    char *buffer = (char *)malloc(NOTE_DATA_SIZE);
    malloc_error_check(buffer);
    char *processed_name = (char *)malloc(NOTE_FILENAME_SIZE);
    malloc_error_check(processed_name);
    get_processed_name(name, processed_name);

    char *filename = (char *)malloc(256);
    malloc_error_check(filename);
    strcpy(filename, NOTES_DIR);
    strcat(filename, processed_name);
    strcat(filename, ".note");

    FILE *note = fopen(filename, "r");
    if (note == NULL) {
        fputs("failed to open file\n", stderr);
        free(filename);
        free(processed_name);
        free(buffer);
        exit(2);
    }
    while (fgets(buffer, NOTE_DATA_SIZE, note) != NULL) {
        printf("%s", buffer);
    }

    free(filename);
    free(processed_name);
    free(buffer);
}

void list_notes() {
    DIR *notes_dir = opendir(NOTES_DIR);
    if (notes_dir == NULL) {
        fputs("failed to open notes directory\n", stderr);
        exit(5);
    }

    struct dirent *note_ent;
    while ((note_ent = readdir(notes_dir)) != NULL) {
        char *raw_name = note_ent->d_name;
        char *dot = strrchr(raw_name, '.');

        if (!dot || strcmp(dot, ".note") != 0) {
            continue;
        }

        if (note_ent->d_type != DT_REG && note_ent->d_name[0] == '.') {
            continue;
        }

        char *processed_name = (char *)malloc(NOTE_FILENAME_SIZE);
        for (int i = 0; i < strlen(raw_name); i++) {
            if (raw_name[i] == '-') processed_name[i] = ' ';
            else processed_name[i] = raw_name[i];
        }

        remove_substr(processed_name, ".note");

        if (note_ent->d_type != DT_REG && note_ent->d_name[0] != '.') {
            free(processed_name);
            continue;
        }

        printf("- %s (filename: '%s')\n", processed_name, raw_name);
        free(processed_name);
    }
}

// TODO: implement an actual text editor (nano is temporary)
void edit_note(char *name) {
    char *filename = (char *)malloc(128);
    malloc_error_check(filename);
    char *processed_name = malloc(sizeof(NOTES_DIR) + strlen(name) + 1);
    malloc_error_check(processed_name);

    get_processed_name(name, processed_name);
    strcpy(filename, NOTES_DIR);
    strcat(filename, processed_name);
    strcat(filename, ".note");

    char command[256] = "nano ";

    strcat(command, filename);
    strcat(command, ".note");
    system(command);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fputs("invalid arguments\n", stderr);
        exit(1);
    }

    if (strcmp(argv[1], "new") == 0) {
        char data[NOTE_DATA_SIZE];
        char name[NOTE_FILENAME_SIZE + sizeof(".note")];

        fputs("name of new note (127 ch. max):  ", stdout);
        fgets(name, NOTE_FILENAME_SIZE, stdin);

        fputs("note text (2047 ch. max): \n", stdout);
        fgets(data, NOTE_DATA_SIZE, stdin);

        new_note(name, data);
    }

    else if (strcmp(argv[1], "read") == 0) {
        char name[NOTE_FILENAME_SIZE + sizeof(".note")];

        fputs("name of note to be read: ", stdout);

        fgets(name, NOTE_FILENAME_SIZE, stdin);

        read_note(name);
    }

    else if (strcmp(argv[1], "list") == 0) {
        list_notes();
    }

    else if (strcmp(argv[1], "edit") == 0) {
        char name[NOTE_FILENAME_SIZE];
        fputs("name of note to edit: ", stdout);
        fgets(name, NOTE_FILENAME_SIZE, stdin);
        edit_note(name);
    }

    else {
        fputs("invalid arguments", stderr);
        exit(1);
    }
    return 0;
}
