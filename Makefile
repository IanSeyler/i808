all:
	gcc -std=c99 -Wall -pedantic -o compiler dev/compiler.c
	gcc -std=c99 -Wall -pedantic -o vm dev/vm.c dev/i808.c
	gcc -std=c99 -Wall -pedantic -o simplevm dev/simplevm.c dev/i808.c
clean:
	rm -f compiler
	rm -f vm
	rm -f simplevm
	rm -f *.bin
