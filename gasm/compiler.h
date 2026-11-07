#include <stddef.h>
#include <inttypes.h>
#include "opcodes.h"
#include "sv.h"

typedef enum {
    PARSE_OK = 0,
    PARSE_EMPTY_LINE,
    PARSE_EMPTY_ARG,
    PARSE_TRAILING_COMMA,
    PARSE_MISSING_COMMA,
    PARSE_UNKNOWN_OP,
    PARSE_MANY_ARGUMENTS,
    PARSE_FEW_ARGUMENTS,
    PARSE_BAD_SIG,
    PARSE_NOT_REG,
    PARSE_BIG_REG,
    PARSE_NAN,
} ParseStatus;

typedef struct {
    ParseStatus status;
    size_t err_col;
} ParseError;

typedef enum {
    OPERAND_REG,
    OPERAND_INT,
    OPERAND_FLOAT,
} OperandKind;

typedef struct {
    OperandKind kind;
    union {
        uint8_t reg;
        int64_t i;
        double  f;
    } as;
} Operand;

typedef struct {
    Operand *items;
    size_t count;
    size_t capacity;
} Operand_Arr;

typedef struct {
    String_View command;
    String_View *items;
    size_t count;
    size_t capacity;
    Operand_Arr args;
} Instr;

int emit_bytecode_from_program(const char* filename, FILE* input_file, FILE* output_file);
