#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "error/error.h"
#include "main.h"
#include "preprocessor.h"
#include "help.h"
#include "error.h"

#define MAX_LABELS 25
#define WORKING_FILE_NAME "preprocessed.asm"

operand_group operand_group_list[] = {
    [MOV] = {3, 2, {{"A", 0x0 << 2, 0x0}, {"B", 0x1 << 2, 0x1}, {"PC", 0x3 << 2, 0x3}}, false, false},
    [ALU] = {2, 2, {{"A", 0x0 << 3, 0x1}, {"B", 0x1 << 3, 0x0}}, false, false},
    [LD] = {2, 2, {{"A", 0x0, 0xff}, {"B", 0x1, 0xff}}, true, false},
    [ST] = {2, 1, {{"A", 0x0, 0xff}, {"B", 0x1, 0xff}}, false, false},
    [JMP] = {0, 1,{}, true, true},
    [CMP] = {2, 1, {{"A", 0x0, 0x0}, {"B", 0x1 << 1, 0x1}}, false, false},
    [BYTE] = {0, 1, {}, true, false}
};

instruction instruction_set[] = {
    {"NOP", 0b0000 << 4, -1},        {"MOV", 0b0001 << 4, MOV},
    {"ADD", 0b1000 << 4, ALU},       {"SUB", 0b1001 << 4, ALU},
    {"SHL", 0b1010 << 4, ALU},       {"SHR", 0b1011 << 4, ALU},
    {"NOT", 0b1100 << 4, ALU},       {"AND", 0b1101 << 4, ALU},
    {"OR", 0b1110 << 4, ALU},        {"XOR", 0b1111 << 4, ALU},
    {"LD", 0b0010 << 4, LD},         {"ST", 0b0011 << 4, ST},
    {"JMP", 0b0100 << 4 | 0x0, JMP}, {"JE", 0b0100 << 4 | 0x1, JMP},
    {"JA", 0b0100 << 4 | 0x2, JMP},   {"JB", 0b0100 << 4 | 0x3, JMP},
    {"CMP", 0b0101 << 4, CMP},       {"BYTE", 0b0000 << 4, BYTE},
};

label labels[MAX_LABELS] = {};
predefined_label predefined_labels[] = {
    {"START", 0, false},
    {"DATA", 255, false},
    {"GLOBAL", 123, false},
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
int parse_number(const char *str);
int find_label_index(char *label);
int find_operand_index(uint8_t operand_group, char* operand, int count);
void label_write(uint8_t value);
void decimal_to_binary_uint8(uint8_t n);
void add_reference(int label_index);
void bubble_sort_lables();
void adjust_label_position();
void link();
void write_hex_file(char* file_name);

int main(int argc, char *argv[]) {
  print_help(argc, argv[0]);

  preprocessor(argv[1], WORKING_FILE_NAME);
  find_labels(WORKING_FILE_NAME, instruction_count, instruction_set);
  move_predefined_labels();
  parse(WORKING_FILE_NAME);

  bubble_sort_lables();
  adjust_label_position();
  link();

  //print_label_array(labels, label_count);

  write_hex_file(argv[1]);
  return 0;
}

void parse_line(char *line, int line_count) {
  validate_arguments_count(line, line_count);

  char buffer[255];
  strcpy(buffer, line);
  char *token = strtok(buffer, " ");
  int count = 0;
  uint8_t byte = 0;
  uint8_t byte2 = 0;
  bool is_byte = false;
  bool is_byte2 = false;
  instruction tmp = {};
  bool is_label = false;
  while (token != NULL) {
    switch (count) {
    case 0:
      if (token[0] == '.') { // labels
        token += 1;          // start pointing from the first character
        label_index = find_label_index(token);
        is_label = true; 
      } else { // mnemotics
        // add error handling if is not a mnemonic
        is_byte = true;
        int tmp_index = find_instruction_index(token);
        if (tmp_index == -1){
          print_error_and_exit(UNKNOWN_MNEMONIC, line_count);
        }
        tmp = instruction_set[tmp_index];
        byte = tmp.opcode;
      }
      break;
    case 1:
      if (is_label) {
        int value = parse_number(token);
        if (value == -1) {
          print_error_and_exit(INVALID_OPERAND, line_count);
        } else if (value > 255) {
          print_error_and_exit(LABEL_ADDRESS_OUT_OF_RANGE, line_count);
        }
        labels[label_index].position = value;
      } else { // is operand for mnemotic
        int ind = find_operand_index(tmp.operand_group, token, count);
        if (ind == -1) {
          print_error_and_exit(INVALID_OPERAND, line_count);
        } else if (ind == -2) { // the operand should be numeric value or label
          if (operand_group_list[tmp.operand_group].operand_count == 0 &&
              !operand_group_list[tmp.operand_group].label_operand) { // when is byte
            int value = parse_number(token);
            if (value == -1) {
              print_error_and_exit(INVALID_OPERAND, line_count);
            } else if (value > 255) {
              print_error_and_exit(LABEL_ADDRESS_OUT_OF_RANGE, line_count);
            }
            byte = value;
          } else{ // label operand
            byte2 = 0x0;
            is_byte2 = true;
            int ind = find_label_index(token);
            add_reference(ind);

          }
        } else { // predefined operand
          byte |= operand_group_list[tmp.operand_group]
                      .operands[ind]
                      .destination_operand;
        }
      }
      break;

    case 2: {
      int ind = find_operand_index(tmp.operand_group, token, count);
      if (ind == -1) {
        print_error_and_exit(INVALID_OPERAND, line_count);
      } else if (ind == -2) { // the operand should be numeric
        int value = parse_number(token);
        if (value == -1){
          print_error_and_exit(INVALID_OPERAND, line_count);
        }
        byte2 = value;
        is_byte2 = true;
      } else { // predefined operand
        byte |= operand_group_list[tmp.operand_group].operands[ind].source_operand;
      }
    } break;
    default:
      break;
    }
    token = strtok(NULL, " ");
    count += 1;
  }
  if(is_byte){
   label_write(byte);
  }
  if(is_byte2){
   label_write(byte2);
  }
}

void validate_arguments_count(char *line, int line_count) {
    int operands = -1;
    char *mnemonic = NULL;
  char buffer[255];
  strcpy(buffer, line);
  char *token = strtok(buffer, " ");
  mnemonic = token;
  while (token != NULL) {
    operands++;
    token = strtok(NULL, " ");
  }
  if (mnemonic[0] != '.' ){
    int index = find_instruction_index(mnemonic);
    int operand_group = instruction_set[index].operand_group;
    if(operand_group == -1){
      return;
    }
    int required = operand_group_list[operand_group].required_operands;
    if (operands > required){
        print_error_and_exit(TO_MANY_ARGUMENTS, line_count);
    } else  if (operands < required){
        
        print_error_and_exit(UNSUFICIENT_ARGUMENTS, line_count);
    }
  }else{
    if (operands > 1){
        print_error_and_exit(TO_MANY_ARGUMENTS, line_count);
    }
  }
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
          if (strcmp(token, labels[i].label) == 0) {
            print_error_and_exit(LABEL_REDEFINITION, line_count);
          }
        }
        strcpy(labels[label_count - 1].label, token);
      }
    }
  }
    // Close the file
    fclose(fp);
}

void print_label_array(label *labels, int size) {
    for (int i = 0; i < size; i++) {
        printf("Label: %s\n", labels[i].label);
        printf("Position: %d\n", labels[i].position);
        printf("Size: %d\n", labels[i].size);
        printf("Code: \n");
        for (int j = 0; j < labels[i].size; j++) {
            printf("%d 0x%x 0b", labels[i].code[j], labels[i].code[j]);
            decimal_to_binary_uint8(labels[i].code[j]);
            printf("\n");
        }
        printf("References count: %d\n", labels[i].ref_count);
        printf("References: \n");
        for (int j = 0; j < labels[i].ref_count; j++) {
            printf("Label index: %s\n", labels[i].reference[j].label);
            printf("Code index: %d\n", labels[i].reference[j].code_ind);
        }
        printf("\n");
    }
}


void move_predefined_labels() {
    int intern_pred_label_count = 0;
    for (int i = 0; i < pred_label_count; i++){
      if( predefined_labels[i].found ){
        intern_pred_label_count += 1;
      }
    }
 
    // shift right existing labels to make space for predefined ones
    for (int i = label_count-1; i >= 0; i--){
      labels[i+intern_pred_label_count] = labels[i];
    }
    for (int i = 0; i < pred_label_count; i++){
       if( predefined_labels[i].found ){
        strcpy(labels[i].label, predefined_labels[i].label);
        labels[i].position = predefined_labels[i].position;
      }
    }
    label_count += intern_pred_label_count;
}

int find_instruction_index(char *mnemonic) {
    for (int i = 0; i < instruction_count; i++) {
      if (strcmp(mnemonic, instruction_set[i].mnemonic) == 0) {
        return i;
      }
    }
    return -1;
}

int find_label_index(char *label) {
    for (int i = 0; i < label_count; i++) {
      if (strcmp(labels[i].label, label) == 0) {
        return i;
      }
    }
    return -1;
}

int parse_number(const char *str) {
    char *endptr;
    long val;
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        val = strtol(str + 2, &endptr, 16);
    } else if (str[0] == '0' && (str[1] == 'b' || str[1] == 'B')) {
        val = strtol(str + 2, &endptr, 2);
    } else if (str[0] == '0') {
        val = strtol(str, &endptr, 8);
    } else {
        val = strtol(str, &endptr, 10);
    }
    if (*endptr != '\0') {
        return -1;
    }
    return (int) val;
}

int find_operand_index(uint8_t operand_group, char* operand, int count){
  int operand_count = operand_group_list[operand_group].operand_count;
  if(operand_count == 0 || (count == 2 && operand_group_list[operand_group].numeric_operand)){
    return -2;
  }    
  for(int i = 0; i < operand_count; i++){
    if(strcmp(operand, operand_group_list[operand_group].operands[i].label) == 0)
    {
      return i;
    }
  }
  return -1;
}

void label_write(uint8_t value){
  labels[label_index].code[labels[label_index].size] = value;
  labels[label_index].size += 1;
}

void decimal_to_binary_uint8(uint8_t n) {
    char binary_num[9] = {0};
    int index = 0;
    while (n > 0) {
        binary_num[index++] = (n % 2 == 0 ? '0' : '1');
        n = n / 2;
    }
    for (int i = 7; i >= 0; i--) {
        if (binary_num[i]) 
            printf("%c", binary_num[i]);
        else
            printf("0");
    }
}

void add_reference(int local_label_index){

  strcpy(
    labels[local_label_index].reference[labels[local_label_index].ref_count].label,
    labels[label_index].label
  );
  labels[local_label_index].reference[labels[local_label_index].ref_count].code_ind = labels[label_index].size+1;
  labels[local_label_index].ref_count += 1;
}

void bubble_sort_lables(){
  label tmp = {};
  for (int i = 0; i < label_count - 1; i++){
    for (int j = 0; j < label_count - i - 1; j++){
      if(labels[j].position >= labels[j+1].position && j != 0){
        tmp = labels[j];
        strcpy(tmp.label, labels[j].label);
        labels[j] = labels[j+1];
        strcpy(labels[j].label, labels[j+1].label);
        labels[j+1] = tmp;
        strcpy(labels[j+1].label, tmp.label);
      }
    }
  }
}

void adjust_label_position(){
  uint8_t adress = labels[0].size;
  for(int i = 1; i < label_count; i++){
    if(labels[i].position != adress){
      labels[i].position = adress;
    }
    adress += labels[i].size;
  }
}

void link(){
  for(int i = 0; i < label_count; i++){
    for(int r = 0; r < labels[i].ref_count; r++){
      ref rr = labels[i].reference[r];
      labels[find_label_index(rr.label)].code[rr.code_ind] = labels[i].position;
    }
  }
}

void write_hex_file(char* file_name) {
    // Create file with the given argument and append 'hex' to the end
    char hex_file_name[strlen(file_name) + 4];
    strcpy(hex_file_name, file_name);
    strcat(hex_file_name, ".hex");
    FILE* hex_file = fopen(hex_file_name, "w");

    // Write the first line of the file
    fprintf(hex_file, "v3.0 hex words plain\n");

    int printed = 0;

    // Write the hex values for each label
    for (int i = 0; i < label_count; i++) {
        // Print hex values for code
        for (int j = 0; j < labels[i].size; j++) {
            fprintf(hex_file, "%02X ", labels[i].code[j]);
            printed += 1;
            if(printed % 16 == 0){
             fprintf(hex_file, "\n");
            }
        }
    }

    // Fill remaining spots with 00
    while (printed != 256) {
        fprintf(hex_file, "00 ");
         printed += 1;
            if(printed % 16 == 0){
             fprintf(hex_file, "\n");
         }
    }

    // Close file
    fclose(hex_file);
}