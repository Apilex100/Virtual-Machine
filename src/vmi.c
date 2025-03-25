#include "./vm.c"


int main(int argc, char ** argv)
{
	if  (argc < 2) {
		fprintf(stderr, "Usage: ./vmi  <input.vm>\n");
		fprintf(stderr, "ERROR: expected input\n");
		exit(1);
	}
	vm_load_program_from_file(&vm, argv[1]);

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
