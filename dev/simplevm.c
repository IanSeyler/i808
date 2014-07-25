/*
simplevm.c - Module file for a simple i808 virtual machine tester
v1.0b - 2007.03.30
Coded by Ian Seyler (iseyler@gmail.com)
*/

/* Obligatory includes */
#include <stdio.h>
#include <stdlib.h>
#include "i808.h"

int main(int argc, char *argv[])
{
	eightbitComputer computer;
	FILE *startfile;
	char *inputfilename;
	char *program_name = argv[0];
	unsigned int cyclestorun = 20, verbose = 1;

	if (argc < 2)
	{
		printf("A Simple i808 VM Tester v0.97b (2007 01 02)\n");
		printf("Coded by Ian Seyler (iseyler@gmail.com)\n\n");
		printf("Usage is %s [program file]\n", program_name);
		printf("Example:\n");
		printf("  %s program.bin		Runs program.bin\n", program_name);
		exit (1);
	}

	inputfilename = argv[1];
	if ((startfile = fopen(inputfilename, "rb")) == NULL)
	{
		printf("\nVM Error:\nError opening file %s\n", inputfilename);
		exit (1);
	}
	fread(computer.mem, 256, 1, startfile);
	fclose(startfile);

	i808reset(&computer);

	printf("Computer Start Status:\n");
	i808show(&computer);
	
	i808compute(&computer, cyclestorun, verbose);

	printf("Computer Finish Status:\n");
	i808show(&computer);

	printf("\n"); //Just to be clean

	return 0;
}
