#ifndef structures_h_
#define structures_h_

#include <stdint.h>
#include <stdint.h>

typedef enum { MOV, ALU, LD, ST, JMP, CMP, BYTE} operands;

typedef struct operand {
    char *label;
    uint8_t destination_operand;
    uint8_t source_operand;
}operand;

typedef struct {
    uint8_t operand_count;
    uint8_t required_operands;
    operand operands[3];
    bool numeric_operand;
    bool label_operand;
}operand_group;

typedef struct instruction {
    char *mnemonic;
    uint8_t opcode;
    int8_t operand_group;
}instruction;

typedef struct ref{
    char label[16];
    uint8_t code_ind;
}ref;

typedef struct{
    char label[16];
    uint8_t position;
    uint8_t size;
    uint8_t code[256];
    ref reference[256];
    uint8_t ref_count;
}label;

typedef struct{
    char label[16];
    uint8_t position;
    bool found;
}predefined_label;

#endif