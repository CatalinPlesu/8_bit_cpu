#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef enum { MOV, ALU, LD, ST, JMP, JE, JA, JB, CMP, BYTE} operands;

typedef struct operand {
    char *label;
    uint8_t destination_operand;
    uint8_t source_operand;
}operand;

typedef struct {
    uint8_t operand_count;
    operand operands[3];
    bool numeric_operand;
    bool label_operand;
}operand_group;

 operand_group operand_group_list[] = {
     [MOV] = {{"A", 0x0 << 2, 0x0}, {"B", 0x1 << 2, 0x1}, {"PC", 0x3 << 2, 0x3}, false, false},
     [ALU] = {{"A", 0x0 << 3, 0x1}, {"B", 0x1 << 3, 0x0}, false, false},
     [LD] = { true, true},
     [ST] = { false, false},
     [JMP] = { true, true},
     [JE] = { true, true},
     [JA] = { true, true},
     [JB] = { true, true},
     [CMP] = { false, false},
     [BYTE] = { true, false}
 };

typedef struct instruction {
    char *mnemonic;
    uint8_t opcode;
    operand operand_group;
}instruction;

instruction instructionSet[] = {
    {"NOP", 0b0000 << 4, -1},
    {"MOV", 0b0001 << 4, MOV},
    {"ADD", 0b1000 << 4, ALU},
    {"SUB", 0b1001 << 4, ALU},
    {"SHL", 0b1010 << 4, ALU},
    {"SHR", 0b1011 << 4, ALU},
    {"NOT", 0b1100 << 4, ALU},
    {"AND", 0b1101 << 4, ALU},
    {"OR", 0b1110 << 4,  ALU},
    {"XOR", 0b1111 << 4, ALU},
    {"LD", 0b0010  << 4, LD},
    {"ST", 0b0011 << 4, ST},
    {"JMP", 0b0100 << 4 | 0x0, JMP},
    {"JE", 0b0100 << 4 | 0x1, JE},
    {"JA", 0b0100 << 4 | 0x2, JA},
    {"JB", 0b0100 << 4 | 0x3, JB},
    {"CMP", 0b0101 << 4, CMP},
    {"BYTE", 0b0101 << 4, BYTE},
};

    struct instruction instructionSet[] = {
    {"NOP", 0b0000 << 4, {}, 0, false, false},
    {"MOV", 0b0001 << 4, {{"A", 0x0 << 2, 0x0}, {"B", 0x1 << 2, 0x1}, {"PC", 0x3 << 2, 0x3}}, 3, false, false},
    {"ADD", 0b1000 << 4, {"A", "B", "BUS"}, 3, false, false},
    {"SUB", 0b1001 << 4, {"A", "B", "BUS"}, 3, false, false},
    {"SHL", 0b1010 << 4, {"A", "B", "BUS"}, 3, false, false},
    {"SHR", 0b1011 << 4, {"A", "B", "BUS"}, 3, false, false},
    {"NOT", 0b1100 << 4, {"A", "B", "BUS"}, 3, false, false},
    {"AND", 0b1101 << 4, {"A", "B", "BUS"}, 3, false, false},
    {"OR", 0b1110 << 4, {"A", "B", "BUS"}, 3, false, false},
    {"XOR", 0b1111 << 4, {"A", "B", "BUS"}, 3, false, false},


const char* predefined_labels = {"start", "start"};

const int instructionCount = sizeof(instructionSet) / sizeof(instructionSet[0]);

FILE* open_file(int argc, char *argv[]);
int parse_file(FILE* file);

int main(int argc, char *argv[]) {
    FILE *file = open_file(argc,argv);
    parse_file(file);
    fclose(file);
    return 0;
}

FILE* open_file(int argc, char *argv[]){
    if (argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        exit(1);
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file %s\n", argv[1]);
        exit(1);
    }
    return file;
}

int parse_file(FILE* file){
    char line[100];
    char *token;
    const char delimiters[] = " \t\n";
    int i, j;

    while (fgets(line, sizeof(line), file)) {
        token = strtok(line, delimiters);
        if (token != NULL) {
            for (i = 0; i < instructionCount; i++) {
                if (strcmp(token, instructionSet[i].mnemonic) == 0) {
                    printf("%s instruction found, opcode: %d\n", instructionSet[i].mnemonic, instructionSet[i].opcode);
                    break;
                }
            }
        }
    }
    return 0;
}
