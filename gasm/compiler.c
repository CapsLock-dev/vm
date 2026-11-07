#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stddef.h>
#include <inttypes.h>

#include "container.h"
#include "compiler.h"
#include "sv.h"

static ParseStatus sv_is_register(String_View sv, uint8_t *num) {
    if (sv.count < 2 || (sv.data[0] != 'r' && sv.data[0] != 'R')) {
        return PARSE_NOT_REG;
    }
    long value = 0;
    for (size_t i = 1; i < sv.count; i++) {
        unsigned char c = (unsigned char)sv.data[i];
        if (!isdigit(c)) return PARSE_NOT_REG;
        value = value * 10 + (c - '0');
        if (value >= REG_COUNT) return PARSE_BIG_REG;
    }

    *num = value;
    return PARSE_OK;
}


static ParseStatus sv_to_double(String_View sv, double *out) {
    char *endptr, *str = strndup(sv.data, sv.count);
    errno = 0;
    double v = strtod(str, &endptr);
    if (errno != ERANGE && endptr != str && *endptr == '\0') {
        *out = v;
        return PARSE_OK;
    }
    free(str);
    return PARSE_NAN;
}

static ParseStatus sv_to_int(String_View sv, int64_t *out) {
    char *endptr, *str = strndup(sv.data, sv.count);
    errno = 0;
    long long val = strtoll(str, &endptr, 10);
    if (errno != ERANGE && endptr != str && *endptr == '\0') {
        *out = val;
        return PARSE_OK;
    }
    free(str);
    return PARSE_NAN;
}


static bool get_opcode(String_View text, Opcode *out) {
    for (size_t i = 0; i < instructions_count; i++) {
        if (sv_eq_cstr(text, instructions[i].name)) {
            *out = instructions[i].op;
            return true;
        }
    }
    return false;
}

static bool parse_command(String_View text, String_View *out) {
    for (size_t i = 0; i < instructions_count; i++) {
        if (sv_eq_cstr(text, instructions[i].name)) {
            *out = sv_from_cstr(instructions[i].sig);
            return true;
        }
    }
    return false;
}

static ParseStatus parse_operand(String_View text, char sig, Operand_Arr *arr_out) {
    Operand out;
    switch(sig) {
        case 'r': {
            uint8_t reg;
            ParseStatus err = sv_is_register(text, &reg);
            if (err != PARSE_OK) return err;
            out.kind = OPERAND_REG;
            out.as.reg = reg;
            da_append(arr_out, out);
            return PARSE_OK;
        }
        case 'i': {
            int64_t value;
            ParseStatus err = sv_to_int(text, &value);
            if (err != PARSE_OK) return err;
            out.kind = OPERAND_INT;
            out.as.i = value;
            da_append(arr_out, out);
            return PARSE_OK;
        }
        case 'f': {
            double value;
            ParseStatus err = sv_to_double(text, &value);
            if (err != PARSE_OK) return err;
            out.kind = OPERAND_FLOAT;
            out.as.f = value;
            da_append(arr_out, out);
            return PARSE_OK;
        }
        case 'n': {
            int64_t value;
            ParseStatus err = sv_to_int(text, &value);
            if (err == PARSE_OK) {
                out.kind = OPERAND_INT;
                out.as.i = value;
                da_append(arr_out, out);
                return PARSE_OK;
            }
            double v;
            err = sv_to_double(text, &v);
            if (err != PARSE_OK) return err;
            out.kind = OPERAND_FLOAT;
            out.as.f = v;
            da_append(arr_out, out);
            return PARSE_OK;
        }
        default: return PARSE_BAD_SIG;
    }
}

static ParseError parse_line(const char *text, Instr *ins) {
    String_View sv = sv_from_cstr(text);
    sv_trim_right(&sv);
    sv_trim_left(&sv);

    if (sv.count == 0) {
        return (ParseError){.status = PARSE_EMPTY_LINE, .err_col = 1};
    }

    size_t i = 0;
    while (i < sv.count && !isspace((unsigned char)sv.data[i])) i++;

    ins->command = sv_from_parts(sv.data, i);
    size_t cmd_col = (size_t)(ins->command.data - text + 1);

    sv.data  += i;
    sv.count -= i;
    sv_trim_left(&sv);

    while (sv.count > 0) {
        sv_trim_left(&sv);
        size_t n = 0;
        while (n < sv.count && sv.data[n] != ',') n++;

        String_View arg = sv_from_parts(sv.data, n);
        sv_trim_right(&arg);

        if (arg.count == 0) {
            ParseError err = {.err_col = (size_t)(arg.data - text + 1)};
            if (n == sv.count) err.status = PARSE_TRAILING_COMMA;
            else               err.status = PARSE_EMPTY_ARG;
            return err;
        }

        for (size_t k = 0; k < arg.count; k++) {
            if (isspace((unsigned char)arg.data[k])) {
                return (ParseError) {
                    .status = PARSE_MISSING_COMMA,
                    .err_col = ((size_t)(arg.data - text) + k + 1)
                };
            }
        }
        da_append(ins, arg);
        bool has_comma = (n < sv.count);
        sv.data  += n;
        sv.count -= n;
        if (!has_comma) break;
        sv.data++;
        sv.count--;
    }

    String_View sig;
    if (!parse_command(ins->command, &sig)) {
        return (ParseError) {
            .status  = PARSE_UNKNOWN_OP,
            .err_col = cmd_col,
        };
    }
    if (ins->count != sig.count) {
        ParseError err = {.err_col = cmd_col};
        if (ins->count < sig.count) err.status = PARSE_FEW_ARGUMENTS;
        else                        err.status = PARSE_MANY_ARGUMENTS;
        return err;
    }
    
    for (size_t k = 0; k < ins->count; k++) {
        ParseStatus err = parse_operand(ins->items[k], sig.data[k], &ins->args);
        if (err != PARSE_OK) {
            return (ParseError){
                .status  = err,
                .err_col = (size_t)(ins->items[k].data - text + 1),
            };
        }
    }

    return (ParseError){.status = PARSE_OK, .err_col = 0};
}

int error_message(ParseError err, size_t err_line, const char* filename) {
    printf("%s:%zu:%zu: ", filename, err_line, err.err_col);
    switch (err.status) {
    case PARSE_OK: return 0;
    case PARSE_EMPTY_LINE: return 0;
    case PARSE_EMPTY_ARG: {
        printf("Error: Empty or missed argument\n");
        return 1;
    }
    case PARSE_TRAILING_COMMA: {
        printf("Error: Trailing comma\n");
        return 1;
    }
    case PARSE_MISSING_COMMA: {
        printf("Error: Missing comma between arguments\n");
        return 1;
    }
    case PARSE_UNKNOWN_OP: {
        printf("Error: Unknown command\n");
        return 1;
    }
    case PARSE_MANY_ARGUMENTS: {
        printf("Error: Too many arguments\n");
        return 1;
    }
    case PARSE_FEW_ARGUMENTS: {
        printf("Error: Too few arguments\n");
        return 1;
    }
    case PARSE_BAD_SIG: {
        printf("ASM Error: Bad signature\n");
        return 1;
    }
    case PARSE_NOT_REG: {
        printf("Error: Expected register in argument\n");
        return 1;
    }
    case PARSE_BIG_REG: {
        printf("Error: Register doesn't exist\n");
        return 1;
    }
    case PARSE_NAN: {
        printf("Error: Expected number in argument\n");
        return 1;
    }
    }
    return 1;
}

static bool write_u8(FILE *out, uint8_t value) {
    return fputc(value, out) != EOF;
}

static bool write_u32(FILE *out, uint32_t value) {
    uint8_t bytes[4] = {
        (uint8_t)(value),
        (uint8_t)(value >> 8),
        (uint8_t)(value >> 16),
        (uint8_t)(value >> 24),
    };
    return fwrite(bytes, 1, sizeof(bytes), out) == sizeof(bytes);
}

static bool write_u64(FILE *out, uint64_t value) {
    uint8_t bytes[8];
    for (size_t i = 0; i < sizeof(bytes); i++) {
        bytes[i] = (uint8_t)(value >> (8 * i));
    }
    return fwrite(bytes, 1, sizeof(bytes), out) == sizeof(bytes);
}

static bool emit_load(FILE *out, const Instr *ins) {
    if (ins->args.count != 2) return false;
    Operand dst = ins->args.items[0];
    Operand src = ins->args.items[1];
    if (dst.kind != OPERAND_REG) return false;
    if (src.kind == OPERAND_INT) {
        if (src.as.i < INT32_MIN || src.as.i > INT32_MAX) return false;
        return write_u8(out, (uint8_t)OP_LOADI)
            && write_u8(out, dst.as.reg)
            && write_u32(out, (uint32_t)(int32_t)src.as.i);
    }
    if (src.kind == OPERAND_FLOAT) {
        uint64_t bits;
        memcpy(&bits, &src.as.f, sizeof bits);
        return write_u8(out, OP_LOADF)
            && write_u8(out, dst.as.reg)
            && write_u64(out, bits);
    }
    return false;
}

static bool emit_rrr(FILE *out, Opcode op, const Instr *ins) {
    if (ins->args.count != 3) return false;
    Operand dst = ins->args.items[0];
    Operand a   = ins->args.items[1];
    Operand b   = ins->args.items[2];
    if (dst.kind != OPERAND_REG
        || a.kind != OPERAND_REG
        || b.kind != OPERAND_REG) {
        return false;
    }
    return write_u8(out, (uint8_t)op)
        && write_u8(out, dst.as.reg)
        && write_u8(out, a.as.reg)
        && write_u8(out, b.as.reg);
}

static bool emit_jmp(FILE *out, const Instr *ins) {
    if (ins->args.count != 1) return false;
    Operand off = ins->args.items[0];
    if (off.kind != OPERAND_INT) return false;
    if (off.as.i < INT32_MIN || off.as.i > INT32_MAX) return false;
    return write_u8(out, OP_JMP)
        && write_u32(out, (uint32_t)(int32_t)off.as.i);
}

static bool emit_jmp_if(FILE *out, Opcode op, const Instr *ins) {
    if (ins->args.count != 2) return false;
    Operand reg = ins->args.items[0];
    Operand off = ins->args.items[1];
    if (reg.kind != OPERAND_REG || off.kind != OPERAND_INT) return false;
    if (off.as.i < INT32_MIN || off.as.i > INT32_MAX) return false;
    return write_u8(out, (uint8_t)op)
        && write_u8(out, reg.as.reg)
        && write_u32(out, (uint32_t)(int32_t)off.as.i);
}

static bool emit_mov(FILE *out, const Instr *ins) {
    if (ins->args.count != 2) return false;

    Operand dst = ins->args.items[0];
    Operand src = ins->args.items[1];

    if (dst.kind != OPERAND_REG || src.kind != OPERAND_REG) return false;

    return write_u8(out, OP_MOV)
        && write_u8(out, dst.as.reg)
        && write_u8(out, src.as.reg);
}

static bool emit_print(FILE *out, const Instr *ins) {
    if (ins->args.count != 1) return false;

    Operand src = ins->args.items[0];

    if (src.kind != OPERAND_REG) return false;

    return write_u8(out, OP_PRINT)
        && write_u8(out, src.as.reg);
}

static bool emit_instr(FILE *out, const Instr *ins) {
    Opcode op;
    if (!get_opcode(ins->command, &op)) return false;
    switch (op) {
        case OP_NOP:
        case OP_HALT:
            return write_u8(out, (uint8_t)OP_HALT);

        case OP_LOADI:
        case OP_LOADF:
            return emit_load(out, ins);

        case OP_ADD:    return emit_rrr(out, (uint8_t)OP_ADD, ins);
        case OP_SUB:    return emit_rrr(out, (uint8_t)OP_SUB, ins);
        case OP_MUL:    return emit_rrr(out, (uint8_t)OP_MUL, ins);
        case OP_DIV:    return emit_rrr(out, (uint8_t)OP_DIV, ins);
        case OP_CMP_LT: return emit_rrr(out, (uint8_t)OP_CMP_LT, ins);

        case OP_JMP:
            return emit_jmp(out, ins);

        case OP_JMP_IF:
            return emit_jmp_if(out, (uint8_t)OP_JMP_IF, ins);
        case OP_JMP_IF_NOT:
            return emit_jmp_if(out, (uint8_t)OP_JMP_IF_NOT, ins);

        case OP_MOV:
            return emit_mov(out, ins);
        case OP_PRINT:
            return emit_print(out, ins);
    }
    
   return false;
}

int emit_bytecode_from_program(const char* filename, FILE* input_file, FILE* output_file) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t lineno = 0;
    while ((read = getline(&line, &len, input_file)) != -1) {
        lineno++;
        Instr ins = {0};
        ParseError err = parse_line(line, &ins);
        if (err.status == PARSE_EMPTY_LINE) {
            free(ins.items);
            free(ins.args.items);
            continue;
        }
        else if (err.status != PARSE_OK) {
            free(ins.items);
            free(ins.args.items);
            return error_message(err, lineno, filename);
        }
        if (!emit_instr(output_file, &ins)) {
            printf("%s:%zu: Error: Cannot emit instruction\n",
                filename, lineno);
            free(ins.items);
            free(ins.args.items);
            return 1;
        }

        free(ins.items);
        free(ins.args.items);
    }

    free(line);
    return 0;
}
