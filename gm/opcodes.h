#include <stddef.h>
#define REG_COUNT 256
typedef enum {
    OP_NOP        = 0,
    OP_HALT       = 1,
    OP_LOADI      = 2,
    OP_LOADF      = 3,
    OP_MOV        = 4,
    OP_ADD        = 5,
    OP_SUB        = 6,
    OP_MUL        = 7,
    OP_DIV        = 8,
    OP_CMP_LT     = 9,
    OP_JMP        = 10,
    OP_JMP_IF     = 11,
    OP_JMP_IF_NOT = 12,
    OP_PRINT      = 13,
} Opcode;

typedef struct {
    const char* name;
    const char* sig;
    Opcode op;
} InstrInfo;


static InstrInfo instructions[] = {
    {"halt",   "",    OP_HALT},
    {"load",   "rn",  OP_LOADI},
    {"cmp_lt", "rrr", OP_CMP_LT},
    {"jz",     "ri",  OP_JMP_IF_NOT},
    {"add",    "rrr", OP_ADD},
    {"jmp",    "i",   OP_JMP},
    {"print",  "r",   OP_PRINT},
};

static const size_t instructions_count
    = sizeof(instructions) / sizeof(instructions[0]);
