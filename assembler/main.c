#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error/error.h"
#include "main.h"
#include "preprocessor.h"
#include "help.h"
#include "error.h"

#define MAX_LABELS 25
#define WORKING_FILE_NAME "preprocessed.asm"

operand_group operand_group_list[] = {
    [MOV] = {3,
             {{"A", 0x0 << 2, 0x0},
              {"B", 0x1 << 2, 0x1},
              {"PC", 0x3 << 2, 0x3}},
             false,
             false},
    [ALU] = {2, {{"A", 0x0 << 3, 0x1}, {"B", 0x1 << 3, 0x0}}, false, false},
    [LD] = {2, {{"A", 0x0, 0xff}, {"B", 0x1, 0xff}}, true, false},
    [ST] = {2, {{"A", 0x0, 0xff}, {"B", 0x1, 0xff}}, false, false},
    [JMP] = {0, {}, true, true},
    [JE] = {0, {}, true, true},
    [JA] = {0, {}, true, true},
    [JB] = {0, {}, true, true},
    [CMP] = {2, {{"A", 0x0, 0x0}, {"B", 0x1 << 1, 0x1}}, false, false},
    [BYTE] = {0, {}, true, false}};

instruction instruction_set[] = {
    {"NOP", 0b0000 << 4, -1},        {"MOV", 0b0001 << 4, MOV},
    {"ADD", 0b1000 << 4, ALU},       {"SUB", 0b1001 << 4, ALU},
    {"SHL", 0b1010 << 4, ALU},       {"SHR", 0b1011 << 4, ALU},
    {"NOT", 0b1100 << 4, ALU},       {"AND", 0b1101 << 4, ALU},
    {"OR", 0b1110 << 4, ALU},        {"XOR", 0b1111 << 4, ALU},
    {"LD", 0b0010 << 4, LD},         {"ST", 0b0011 << 4, ST},
    {"JMP", 0b0100 << 4 | 0x0, JMP}, {"JE", 0b0100 << 4 | 0x1, JE},
    {"JA", 0b0100 << 4 | 0x2, JA},   {"JB", 0b0100 << 4 | 0x3, JB},
    {"CMP", 0b0101 << 4, CMP},       {"BYTE", 0b0101 << 4, BYTE},
};

label labels[MAX_LABELS] = {};
predefined_label predefined_labels[] = {
    {"START", 0, false},
    {"DATA", 255, false},
};

const int pred_label_count =
    sizeof(predefined_labels) / sizeof(predefined_labels[0]);
const int instruction_count =
    sizeof(instruction_set) / sizeof(instruction_set[0]);
int label_count = 0;
uint8_t label_index = 0;

void find_labels(const char *file_path, int instruction_count,
                 instruction *instruction_set);
void print_label_array(label *labels, int size);
void parse(char *file);
void parse_line(char *line, int line_count);
void validate_arguments_count(char *line, int line_count);
int find_instruction_index(char *mnemonic);
void move_predefined_labels();

int main(int argc, char *argv[]) {
  print_help(argc, argv[0]);

  preprocessor(argv[1], WORKING_FILE_NAME);
  find_labels(WORKING_FILE_NAME, instruction_count, instruction_set);
  move_predefined_labels();
  parse(WORKING_FILE_NAME);

  print_label_array(labels, label_count);

  return 0;
}

void parse_line(char *line, int line_count) {
  //char *token = strtok(line, " ");
  //while (token != NULL) {
  //  printf("%s\n", token);
  //  token = strtok(NULL, " ");
  //}
  validate_arguments_count(line, line_count);
}

void validate_arguments_count(char *line, int line_count) {
    int operands = -1;
    char *mnemonic = NULL;
  char *token = strtok(line, " ");
  mnemonic = token;
  while (token != NULL) {
    operands++;
    token = strtok(NULL, " ");
  }
  printf("%s, arguments: %d\n", mnemonic, operands);
}

void parse(char *file) {
  FILE *fp = fopen(file, "r");
  if (!fp) {
    printf("Error: Unable to open file %s\n", file);
    return;
  }

  char line[256];
  int line_count = 1;
  while (fgets(line, sizeof(line), fp)) {
    // Remove newline character from the end of the line
    line[strcspn(line, "\r\n")] = 0;
    parse_line(line, line_count);
    line_count += 1;
  }

  // Close the file
  fclose(fp);
}

void find_labels(const char *file_path, int instruction_count,
                 instruction *instruction_set) {
  int adress = 0;
  FILE *fp = fopen(file_path, "r");
  if (!fp) {
    printf("Error: Unable to open file %s\n", file_path);
    return;
  }

  char line[256];
  int labels_found = 0;
  int line_count = 0;
  while (fgets(line, sizeof(line), fp)) {
    line_count += 1;
    // Remove newline character from the end of the line
    line[strcspn(line, "\r\n")] = 0;

    // Split the line into tokens
    char *token = strtok(line, " ");
    // if it-s a label will search if it is predefined or not
    bool label_found = false;
    if (token[0] == '.') {
      labels_found += 1;
      if (labels_found >= MAX_LABELS)
        print_error_and_exit(LABELS_LIMIT_EXCEEDED, -1);
      token += 1;
      for (int i = 0; i < pred_label_count; i++) {
        if (label_found)
          break;
        if (strcmp(token, predefined_labels[i].label) == 0) {
          predefined_labels[i].found = true;
          label_found = true;
        }
      }

      if (!label_found) {
        label_count += 1;
        for (int i = 0; i < label_count; i++) {
            printf("%s %s", token, labels[i].label);
          if (strcmp(token, labels[i].label) == 0) {
            print_error_and_exit(LABEL_REDEFINITION, line_count);
          }
        }
        strcpy(labels[label_count - 1].label, token);
      }
    }

    // Close the file
    fclose(fp);
  }
}

void print_label_array(label *labels, int size) {
  for (int i = 0; i < size; i++) {
    printf("Label: %s\n", labels[i].label);
    printf("Position: %d\n", labels[i].position);
    printf("Code: ");
    for (int j = 0; j < labels[i].size; j++) {
      printf("%d ", labels[i].code[j]);
    }
    printf("\nSize: %d\n", labels[i].size);
    printf("\n");
  }
}

void move_predefined_labels() {

  for (int i = 0; i < pred_label_count; i++) {
    for (int j = 0; j < label_count; j++) {
      if (strcmp(predefined_labels[i].label, labels[j].label) == 0) {
        predefined_labels[i].found = true;
        labels[j].position = predefined_labels[i].position;
        break;
      }
    }
  }
  int predefined_index = 0;
  int non_predefined_index = pred_label_count;
  while (predefined_index < pred_label_count &&
         non_predefined_index < label_count) {
    while (predefined_index < pred_label_count &&
           predefined_labels[predefined_index].found) {
      predefined_index++;
    }
    while (non_predefined_index < label_count &&
           labels[non_predefined_index].position != -1) {
      non_predefined_index++;
    }
    if (predefined_index < pred_label_count &&
        non_predefined_index < label_count) {
      labels[predefined_index] = labels[non_predefined_index];
      labels[non_predefined_index].position = -1;
      predefined_labels[predefined_index].found = true;
      predefined_index++;
      non_predefined_index++;
    }
  }
}

int find_instruction_index(char *mnemonic) {
for (int i = 0; i < instruction_count; i++) {
if (strcmp(mnemonic, instruction_set[i].mnemonic) == 0) {
return i;
}
}
return -1;
}