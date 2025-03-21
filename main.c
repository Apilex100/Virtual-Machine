#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof((xs)[0]))

#define VM_STACK_CAPACITY 1024
#define BM_PROGRAM_CAPACITY 1024
#define VM_EXECUTION_LIMIT 79

typedef enum {
	ERR_OK = 0,
	ERR_STACK_OVERFLOW,
	ERR_STACK_UNDERFLOW,
	ERR_ILLEGAL_INST,
	ERR_ILLEGAL_INST_ACCESS,
	ERR_DIV_BY_ZERO,
} Err;

const char *err_as_cstr(Err err) {
	switch (err) {
		case ERR_OK:
			return "ERR_OK";
		case ERR_STACK_OVERFLOW:
			return "ERR_STACK_OVERFLOW";
		case ERR_STACK_UNDERFLOW:
			return "ERR_STACK_UNDERFLOW";
		case ERR_ILLEGAL_INST:
			return "ERR_ILLEGAL_INST";
		case ERR_DIV_BY_ZERO:
			return "ERR_DIV_BY_ZERO";
		case ERR_ILLEGAL_INST_ACCESS:
			return "ERR_ILLEGAL_INST_ACCESS";
		default:
			assert(0 && "err_as_cstr: Unreachable");
	}
}

typedef int64_t Word;

typedef enum {
	INST_PUSH,
	INST_DUP,
	INST_PLUS,
	INST_MINUS,
	INST_MULT,
	INST_DIV,
	INST_JMP,
	INST_JMP_IF,
	INST_EQ,
	INST_HALT,
	INST_PRINT_DEBUG,
} Inst_Type;

const char *inst_type_as_cstr(Inst_Type type)
{
	switch (type) {
		case INST_PUSH: return "INST_PUSH";
		case INST_DUP: return "INST_DUP";
		case INST_PLUS: return "INST_PLUS";
		case INST_MINUS: return "INST_MINUS";
		case INST_MULT: return "INST_MULT";
		case INST_DIV: return "INST_DIV";
		case INST_JMP: return "INST_JMP";
		case INST_JMP_IF: return "INST_JMP_IF";
		case INST_EQ: return "INST_EQ";
		case INST_HALT: return "INST_HALT";
		case INST_PRINT_DEBUG: return "INST_PRINT_DEBUG";
		default: assert(0 && "inst_type_as_cstr: Unreachable");
	}
}

typedef struct {
	Inst_Type type;
	Word operand;
} Inst;

typedef struct {
	Word stack[VM_STACK_CAPACITY];
	Word stack_size;

	Inst program[BM_PROGRAM_CAPACITY];
	Word program_size;
	Word ip;

	int halt;
} Vm;

#define MAKE_INST_PUSH(value) { .type = INST_PUSH, .operand = (value) }
#define MAKE_INST_PLUS() { .type = INST_PLUS }
#define MAKE_INST_MINUS() { .type = INST_MINUS }
#define MAKE_INST_MULT() { .type = INST_MULT }
#define MAKE_INST_DIV() { .type = INST_DIV }
#define MAKE_INST_JMP(addr) { .type = INST_JMP, .operand = (addr) }
#define MAKE_INST_DUP(addr) { .type = INST_DUP, .operand = (addr) }
#define MAKE_INST_JMP_IF { .type = INST_JMP_IF}
#define MAKE_INST_HALT { .type = INST_HALT }

Err vm_get_stack_frame(const Vm *vm, Word addr)
{

}

Err vm_execute_inst(Vm *vm)
{
	if (vm->ip < 0 || vm->ip >= vm->program_size) {
		return ERR_ILLEGAL_INST_ACCESS;
	}
	
	Inst inst = vm->program[vm->ip];

	switch (inst.type) {
		case INST_PUSH:
			if (vm->stack_size >= VM_STACK_CAPACITY) {
				return ERR_STACK_OVERFLOW;
			}
			vm->stack[vm->stack_size] = inst.operand;
			vm->stack_size += 1;
			vm->ip += 1;
			break;

		case INST_PLUS:
			if (vm->stack_size < 2) {
				return ERR_STACK_UNDERFLOW;
			}
			vm->stack[vm->stack_size - 2] += vm->stack[vm->stack_size - 1];
			vm->stack_size -= 1;
			vm->ip += 1;
			break;

		case INST_MINUS:
			if (vm->stack_size < 2) {
				return ERR_STACK_UNDERFLOW;
			}
			vm->stack[vm->stack_size - 2] -= vm->stack[vm->stack_size - 1];
			vm->stack_size -= 1;
			vm->ip += 1;
			break;
			
		case INST_MULT:
			if (vm->stack_size < 2) {
				return ERR_STACK_UNDERFLOW;
			}
			vm->stack[vm->stack_size - 2] *= vm->stack[vm->stack_size - 1];
			vm->stack_size -= 1;
			vm->ip += 1;
			break;

		case INST_DIV:
			if (vm->stack_size < 2) {
				return ERR_STACK_UNDERFLOW;
			}
			if (vm->stack[vm->stack_size - 1] == 0) {
				return ERR_DIV_BY_ZERO;
			}
			vm->stack[vm->stack_size - 2] /= vm->stack[vm->stack_size - 1];
			vm->stack_size -= 1;
			vm->ip += 1;
			break;
		
		case INST_JMP:
			vm->ip = inst.operand;
			break;

		case INST_HALT:
			vm->halt = 1;
			break;

		case INST_EQ:
			if (vm->stack_size < 2) {
				return ERR_STACK_UNDERFLOW;
			}
			vm->stack[vm->stack_size - 2] = vm->stack[vm->stack_size - 1] == vm->stack[vm->stack_size - 2];
			vm->stack_size -= 1;
			vm->ip += 1;
			break;

		case INST_JMP_IF:
			if (vm->stack_size < 1) {
				return ERR_STACK_UNDERFLOW;
			}
			if (vm->stack[vm->stack_size - 1]) {
				vm->stack_size -= 1;
				vm->ip = inst.operand;
			}
			else {
				vm->ip += 1;
			}
			break;

		case INST_PRINT_DEBUG:
			if (vm->stack_size < 1) {
				return ERR_STACK_UNDERFLOW;
			}
			printf("%ld\n", vm->stack[vm->stack_size]);
			vm->stack_size -= 1;
			vm->ip += 1;
			break;

		case INST_DUP:
			if (vm->stack_size - inst.operand <= 0) {

			}
			break;

		default:
			return ERR_ILLEGAL_INST;
	}
	return ERR_OK;
}

void vm_dump_stack(FILE *stream, const Vm *vm)
{
	fprintf(stream, "Stack:\n");
	if (vm->stack_size > 0) {
		for (Word i = 0; i < vm->stack_size; ++i) {
			fprintf(stream, "	%ld\n", vm->stack[i]);
		}
	}
	else {
		fprintf(stream, "	[EMPTY]\n");
	}
}

void vm_load_program_from_memory(Vm *vm, Inst *program, size_t program_size)
{
	assert(program_size < BM_PROGRAM_CAPACITY);
	memcpy(vm->program, program, sizeof(program[0]) * program_size);
	vm->program_size = program_size;
}

Vm vm = {0};
Inst program[] = {
	MAKE_INST_PUSH(0),
	MAKE_INST_PUSH(0),
};

int main()
{
	vm_load_program_from_memory(&vm, program, ARRAY_SIZE(program));

	vm_dump_stack(stderr, &vm);
	for (int i = 0; i < VM_EXECUTION_LIMIT && !vm.halt; ++i) {
		Err err = vm_execute_inst(&vm);
		vm_dump_stack(stdout, &vm);
		if (err != ERR_OK) {
			fprintf(stderr, "ERROR: %s\n", err_as_cstr(err));
			exit(1);
		}
	}
	return 0;
}
