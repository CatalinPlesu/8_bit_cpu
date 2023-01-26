#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

static void create_preprocessed_file_copy(char* filename, char* tmp_file){
    FILE *original_file;
    original_file = fopen(filename, "r");
    FILE *working_copy;
    working_copy = fopen(tmp_file, "w");

    char ch = fgetc(original_file);

    while (ch != EOF) {
        ch = toupper(ch);
        fputc(ch, working_copy);
        ch = fgetc(original_file);
    }

    fclose(original_file);
    fclose(working_copy);
}

void clean_up_file(char* tmp_file){
    char temp_file[strlen(tmp_file) + 5];
    strcpy(temp_file, tmp_file);
    strcat(temp_file, ".tmp");

    FILE* in = fopen(tmp_file, "r");
    FILE* out = fopen(temp_file, "w");
    int c, prev_c = ' ';
    bool newline_written = true; // flag to keep track of whether newline was written

    while ((c = fgetc(in)) != EOF) {
        if (!isspace(c) || (isspace(c) && !isspace(prev_c) && c != '\n')) {
            fputc(c, out);
            if(c == '\n') newline_written = true;
            else newline_written = false;
        }
        else if(c == '\n' && !newline_written) {
            fputc(c, out);
            newline_written = true;
        }
        prev_c = c;
    }
    fclose(in);
    fclose(out);

    remove(tmp_file);
    rename(temp_file, tmp_file);
}
void remove_comments_from_file(char* tmp_file){
char temp_file[strlen(tmp_file) + 5];
    strcpy(temp_file, tmp_file);
    strcat(temp_file, ".tmp");

    FILE* in = fopen(tmp_file, "r");
    FILE* out = fopen(temp_file, "w");
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), in)) {
        if (buffer[0] != ';') {
            char *p = strchr(buffer, ';');
            if (p != NULL) {
                *p = '\n';
                *(p+1) = '\0';
            }
            fputs(buffer, out);
        }
    }
    fclose(in);
    fclose(out);

    remove(tmp_file);
    rename(temp_file, tmp_file);
}

void preprocessor(char* filename, char* tmp_file){
    create_preprocessed_file_copy(filename, tmp_file);
    clean_up_file(tmp_file);
    remove_comments_from_file(tmp_file);
}
