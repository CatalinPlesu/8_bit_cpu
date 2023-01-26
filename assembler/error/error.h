#ifndef error_h_
#define error_h_

typedef enum {
  TO_MANY_ARGUMENTS,
  UNSUFICIENT_ARGUMENTS,
  INVALID_OPERAND,
  UNKNOWN_MNEMONIC,
  LABEL_REDEFINITION,
  UNKNOWN_LABEL,
  LABELS_LIMIT_EXCEEDED,
  OUT_OF_MEMORY,
  UNKNOWN_ERROR
} error_codes;

void print_error_and_exit(error_codes error_code, int line);

#endif