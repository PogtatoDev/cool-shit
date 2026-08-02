#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STRFORMAT "%.*s"
#define STRARGS(s) (int)(s).count, (s).text

typedef struct {
	char *text;
	size_t count;
} StringView;

StringView cstr_to_sv(char *cstr) {
	return (StringView){.text = cstr, .count = strlen(cstr)};
}

void sv_init_count(StringView *sv) {
	if (sv->text != NULL)
		sv->count = strlen(sv->text);
}

size_t sv_character_occurence(StringView sv, char c, bool case_sensitive) {
	size_t n = 0;
	if (case_sensitive) {
		for (size_t i = 0; i < sv.count; i++) {
			if (sv.text[i] == c) n++;
		}

		return n;
	}

	for (size_t i = 0; i < sv.count; i++) {
		if (sv.text[i] == toupper(c) || sv.text[i] == tolower(c)) n++;
	}

	return n;
}

void sv_remove_character(StringView sv_in, StringView *sv_out, char c,
						 bool case_sensitive) {

	size_t out_idx = 0;
	size_t occurrences = sv_character_occurence(sv_in, c, case_sensitive);
	sv_out->text = (char *)malloc(sv_in.count - occurrences);
	if (sv_out->text == NULL) {
		sv_out->count = 0;
		return;
	}
	if (case_sensitive) {
		for (size_t i = 0; i < sv_in.count; i++) {
			if (sv_in.text[i] != c) {
				sv_out->text[out_idx] = sv_in.text[i];
				out_idx++;
			}
		}

		sv_out->count = out_idx;
		return;
	}

	for (size_t i = 0; i < sv_in.count; i++) {
		if (sv_in.text[i] != toupper(c) && sv_in.text[i] != tolower(c)) {
			sv_out->text[out_idx] = sv_in.text[i];
			out_idx++;
		}
	}

	sv_out->count = out_idx;
	return;
}

void sv_chop_left(StringView *sv, size_t n) {
	if (sv->count < n) n = sv->count;
	sv->count -= n;
	sv->text += n;
}

void sv_chop_right(StringView *sv, size_t n) {
	if (sv->count < n) n = sv->count;
	sv->count -= n;
}

void sv_trim(StringView *sv) {
    while (sv->count > 0 && isspace((uint8_t)sv->text[sv->count - 1])) {
        sv_chop_right(sv, 1);
    }
}

void sv_to_upper(StringView sv_in, StringView *sv_out) {

	sv_out->text = (char *)malloc(sv_in.count + 1);

	for (size_t i = 0; i < sv_in.count; i++)
		if (sv_in.text[i] >= 'a' && sv_in.text[i] <= 'z')
			sv_out->text[i] = sv_in.text[i] - 32;
		else

			sv_out->text[i] = sv_in.text[i];
	sv_out->count = sv_in.count;
	sv_out->text[sv_in.count + 1] = '\0';
}

void sv_to_lower(StringView sv_in, StringView *sv_out) {
	sv_out->text = (char *)malloc(sv_in.count + 1);
	if (sv_out->text == NULL) return;
	for (size_t i = 0; i < sv_in.count; i++)
		if (sv_in.text[i] >= 'A' && sv_in.text[i] <= 'Z')
			sv_out->text[i] = sv_in.text[i] + 32;
		else
			sv_out->text[i] = sv_in.text[i];

	sv_out->text[sv_in.count + 1] = '\0';
}

void sv_to_cstr(StringView sv_in, char *dest_cstr) {
	size_t dest_size = sv_in.count + 1;
	for (size_t i = 0; i < dest_size; i++) { dest_cstr[i] = sv_in.text[i]; }
	dest_cstr[sv_in.count] = '\0';
}

StringView sv_tok(StringView *sv, char delim) {
	while (sv->count < 0 && sv->text[0] == delim)
		sv_chop_left(sv, 1);
	
	if (sv->count == 0)
		return (StringView) {.text = NULL, .count = 0};

	StringView tok = {.text = sv->text, .count = 0};


    while (tok.count < sv->count && tok.text[tok.count] != delim)
        tok.count++;
	
	sv_chop_left(sv, tok.count);
	return tok;
}

bool sv_compare(StringView sv1, StringView sv2) {
	if (sv1.count != sv2.count) return false;
	return (bool)strcmp(sv1.text, sv2.text);
}
