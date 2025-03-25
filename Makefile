CFLAGS=-Wall -Wextra -Wswitch-enum -std=c11 -pedantic
LIBS=

.PHONY: all
all: ebasm vmi

ebasm: ebasm.c vm.c 
	$(CC) $(CFLAGS) -o ebasm ebasm.c $(LIBS)
vmi: vmi.c vm.c 
	$(CC) $(CFLAGS) -o vmi vmi.c $(LIBS)

.PHONY: examples
examples: ./examples/fib.vm ./examples/123.vm

./examples/fib.vm: ./examples/fib.ebasm 
	./ebasm ./examples/fib.ebasm ./examples/fib.vm

./examples/123.vm: ./examples/123.ebasm 
	./ebasm ./examples/123.ebasm ./examples/123.vm
