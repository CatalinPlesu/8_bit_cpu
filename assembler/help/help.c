#include <stdio.h>
#include <stdlib.h>
#include "help.h"

void print_help(int argc, char *program_file) {
    if (argc != 2) {
        printf("Usage: %s filename\n", program_file);

        printf("Instruction Set:\n");
        printf("NOP: No operation\n");
        printf("MOV r r: Move data between registers A, B and PC\n");
        printf("ADD r r: Add the values of registers A and B\n");
        printf("SUB r r: Subtract the value of register B from register A\n");
        printf("SHL r r: Shift the bits of register A left\n");
        printf("SHR r r: Shift the bits of register A right\n");
        printf("NOT r r: Perform bitwise NOT on register A\n");
        printf("AND r r: Perform bitwise AND on registers A and B\n");
        printf("OR r r: Perform bitwise OR on registers A and B\n");
        printf("XOR r r: Perform bitwise XOR on registers A and B\n");
        printf("LD r value: Load data from memory into registers A and B\n");
        printf("ST r: Store data from registers A and B into memory will use as addres the other register\n");
        printf("JMP addres/label: Jump to a specified location in memory\n");
        printf("JE addres/label: Jump to a specified location if A and B are equal\n");
        printf("JA addres/label: Jump to a specified location if A is greater than B\n");
        printf("JB addres/label: Jump to a specified location if B is greater than A\n");
        printf("CMP r r: Compare the values of registers A and B\n");
        printf("BYTE: Store a byte-sized value in memory\n");
        printf("Lables start with . and you can specify thier location in memory\n");
        printf("Must include .START where your program will begin\n");
        printf(".DATA will be placed at the bottom of the file\n");

        exit(1);
    }
}
