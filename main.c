#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define VM_STACK_CAPACITY 1024

typedef enum {
	ERR_OK = 0,
	ERR_STACK_OVERFLOW,
	ERR_STACK_UNDERFLOW,
	ERR_ILLEGAL_INST,
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
		default:
			assert(0 && "err_as_cstr: Unreachable");
	}
}

typedef int64_t Word;

typedef struct {
	Word stack[VM_STACK_CAPACITY];
	size_t stack_size;
	Word ip;
	int halt;
} Vm;

typedef enum {
	INST_PUSH,
	INST_PLUS,
	INST_MINUS,
	INST_MULT,
	INST_DIV,
	INST_JMP,
	INST_HALT,
} Inst_Type;

const char *inst_type_as_cstr(Inst_Type type)
{
	switch (type) {
		case INST_PUSH: return "INST_PUSH";
		case INST_PLUS: return "INST_PLUS";
		case INST_MINUS: return "INST_MINUS";
		case INST_MULT: return "INST_MULT";
		case INST_DIV: return "INST_DIV";
		case INST_JMP: return "INST_JMP";
		case INST_HALT: return "INST_HALT";
		default: assert(0 && "inst_type_as_cstr: Unreachable");
	}
}

typedef struct {
	Inst_Type type;
	Word operand;
} Inst;


#define MAKE_INST_PUSH(value) { .type = INST_PUSH, .operand = (value) }
#define MAKE_INST_PLUS() { .type = INST_PLUS }
#define MAKE_INST_MINUS() { .type = INST_MINUS }
#define MAKE_INST_MULT() { .type = INST_MULT }
#define MAKE_INST_DIV() { .type = INST_DIV }
#define MAKE_INST_JMP(addr) { .type = INST_JMP, .operand = (addr) }
#define MAKE_INST_HALT { .type = INST_HALT }

Err vm_execute_inst(Vm *vm, Inst inst)
{
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
			if (inst.operand < 0 || inst.operand > )

		default:
			return ERR_ILLEGAL_INST;
	}
	return ERR_OK;
}

void vm_dump(FILE *stream, const Vm *vm)
{
	fprintf(stream, "Stack:\n");
	if (vm->stack_size > 0) {
		for (size_t i = 0; i < vm->stack_size; ++i) {
			fprintf(stream, "	%ld\n", vm->stack[i]);
		}
	}
	else {
		fprintf(stream, "	[EMPTY]\n");
	}
}

#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof((xs)[0]))

Vm vm = {0};
Inst program[] = {
	MAKE_INST_PUSH(1),
	MAKE_INST_PUSH(2),
	MAKE_INST_PLUS(),
	/*MAKE_INST_PUSH(3),*/
	/*MAKE_INST_PUSH(4),*/
	/*MAKE_INST_MINUS(),*/
	/*MAKE_INST_MULT(),*/
	MAKE_INST_PUSH(0),
	MAKE_INST_DIV(),
};

int main()
{
	vm_dump(stderr, &vm);
	while(!vm.halt) {
		printf("	%s\n", inst_type_as_cstr(program[vm.ip].type));
		Err err = vm_execute_inst(&vm, program[vm.ip]);
		vm_dump(stdout, &vm);
		if (err != ERR_OK) {
			fprintf(stderr, "ERROR: %s\n", err_as_cstr(err));
			vm_dump(stderr, &vm);
			exit(1);
		}
	}
	return 0;
}
