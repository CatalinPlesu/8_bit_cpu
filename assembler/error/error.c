#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void print_error_and_exit(error_codes error_code, int line) {
  char *error_message;
  switch (error_code) {
  case TO_MANY_ARGUMENTS:
    error_message = "Too many arguments provided";
    break;
  case UNSUFICIENT_ARGUMENTS:
    error_message = "Insufficient arguments provided";
    break;
  case INVALID_OPERAND:
    error_message = "Invalid operand provided";
    break;
  case UNKNOWN_MNEMONIC:
    error_message = "Unknown mnemonic provided";
    break;
  case LABEL_REDEFINITION:
    error_message = "Label has been redefined";
    break;
  case UNKNOWN_LABEL:
    error_message = "Unknown label provided";
    break;
  case LABELS_LIMIT_EXCEEDED:
    error_message = "The label limit was exceded";
    break;
  case LABEL_ADDRESS_OUT_OF_RANGE:
    error_message = "Label address is out of range";
    break;
  case OUT_OF_MEMORY:
    error_message = "Out of memory";
    break;
  default:
    error_message = "Unknown error occurred";
    break;
  }
  printf("\033[0;31m"); // Change text color to red
  printf("Error on line %d: %s\n", line, error_message);
  printf("\033[0m"); // Reset text color
  exit(1);
}