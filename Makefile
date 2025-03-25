CFLAGS=-Wall -Wextra -Wswitch-enum -std=c11 -pedantic
LIBS=

.PHONY: all
all: ebasm vmi

ebasm: ./src/ebasm.c ./src/vm.c 
	$(CC) $(CFLAGS) -o ebasm ./src/ebasm.c $(LIBS)
vmi: ./src/vmi.c ./src/vm.c 
	$(CC) $(CFLAGS) -o vmi ./src/vmi.c $(LIBS)

.PHONY: examples
examples: ./examples/fib.vm ./examples/123.vm

./examples/fib.vm: ebasm ./examples/fib.ebasm 
	./ebasm ./examples/fib.ebasm ./examples/fib.vm

./examples/123.vm: ebasm ./examples/123.ebasm 
	./ebasm ./examples/123.ebasm ./examples/123.vm
