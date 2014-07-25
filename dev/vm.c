/*
vm.c - Module file for i808 virtual machine tester
v1.0b - 2007.11.15
Coded by Ian Seyler (iseyler@gmail.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "i808.h"

/* Version information strings */
char VERSIONINFO1[] = "i808 VM Tester v1.0b";
char VERSIONINFO2[] = "Compiled on 2007.11.15, based on i808 spec 2006.08.30";

/* Function prototypes. */
int randint(int min, int max);
void printhelp();

/* Global variables */
eightbitComputer computer;
FILE *startfile;
char *inputfile;
char *program_name;
char statefile_name[40];

int main(int argc, char *argv[])
{
	unsigned char tchar;
	unsigned int cyclestorun = 20, verbose = 0, randomness = 0, timed = 0;
	unsigned int loadstate = 0, savestate = 0, tint;
	time_t tval1, tval2; // used for keeping track of running time

	program_name = argv[0];

	srand(time(NULL)); // randomize
	for (tint=0; tint<5; tint++)
		tint = randint(0, 255); // clear out the first few random numbers

	if (argc < 2 || argv[1][0] == '/' || argv[1][0] == '-') // make sure there is at least one argument (filename)
	{
		printhelp();
		exit (1);
	}

	// Parse the arguments
	for(tint=1; tint<argc; tint++)
	{
		if (argv[tint][0] == '-' || argv[tint][0] == '/') // accept -arg and /arg
		{
			if (argv[tint][1] == 'v') // check for verbose mode
			{
				verbose = 1;
			}
			else if (argv[tint][1] == 'c') // check for cycles
			{
				if (tint+1 < argc) // make sure there is something after -c
				{
					if (isdigit(argv[tint+1][0]))
					{
						cyclestorun = atoi(argv[tint+1]);
						tint = tint + 1;
					}
					else
					{
						printf("\nVM Error:\nError parsing cycles argument\n");
						exit (1);
					}
				}
				else
				{
					printf("\nVM Error:\nError parsing cycles argument\n");
					exit (1);
				}
			}
			else if (argv[tint][1] == 'l') // load state
			{
				loadstate = 1;
			}
			else if (argv[tint][1] == 's') // save state
			{
				savestate = 1;
			}
			else if (argv[tint][1] == '?' || argv[tint][1] == 'h') // check for help
			{
				printhelp();
				exit (1);
			}
			else if (argv[tint][1] == 'r') // activate randomness (hidden feature)
			{
				randomness = 1;
			}
			else if (argv[tint][1] == 't') // activate timing (hidden feature)
			{
				timed = 1;
			}
			else
			{
				printf("\nVM Error:\nInvalid argument '%s'\n", argv[tint]);
				exit (1);			
			}
		}
		else
		{
			// Not a valid arguement.. skip it.
		}
	}

	i808reset(&computer);

	inputfile = argv[1];
	if ((startfile = fopen(inputfile, "rb")) == NULL)
	{
		printf("\nVM Error:\nError opening file %s\n", inputfile);
		exit (1);
	}
	// detect how large the file is. use this to determine what architecture.
	fread(computer.mem, 256, 1, startfile);
	fclose(startfile);

	strcpy(statefile_name, inputfile);
	strcat(statefile_name, ".state");

	if (randomness == 1) // create a random computer, fill memory with randomness
		for (tint=0; tint<256; tint++)
			computer.mem[tint] = randint(0, 255);

//	printf("Seeding random data to IO ports\n");
//	for (tint=0; tint<16; tint++)
//		i808writeport(&computer, tint, randint(0, 255));

//	i808writeport(&computer, 2, 0xBE);

//	i808show(&computer);

	if (loadstate == 1)
		i808loadstate(&computer, statefile_name);

	time(&tval1); // Base time
	for (tint=0; tint<cyclestorun; tint++)
	{
		if (verbose == 1) printf("\n");
		i808compute(&computer, 1, verbose);
		if (i808checkportflag(&computer, 15) == 1)
		{
			tchar = i808readport(&computer, 15);
			if (verbose == 1)
				printf("\tCPU -> Port %d : ", 15);
			printf("%c", tchar);
//			printf("%02X", tchar);
		}
	}
	time(&tval2); // Running time

	i808show(&computer);

	if (timed == 1)
		printf("\nElapsed program time in seconds: %.0lf", difftime(tval2,tval1));

	if (savestate == 1)
		i808savestate(&computer, statefile_name);

	printf("\n"); // Just to be clean

	return (0);
}


/*========================================================*
 * Function: randint()
 * Purpose : generate a random number between min and max
 * Entry   : int min = Minimum number
 *         : int max = Maximum number
 * Returns : random number between min and max inclusive
 *========================================================*/

int randint(int min, int max)
{
	return (min + (rand() % ((max+1) - min)));
}


/*========================================================*
 * Function: printhelp()
 * Purpose : Print the help text
 * Entry   : None
 * Returns : None
 *========================================================*/

void printhelp()
{
	printf("%s\n", VERSIONINFO1);
	printf("%s\n", VERSIONINFO2);
	printf("Coded by Ian Seyler (iseyler@gmail.com)\n\n");
	printf("Usage: %s [program file] [options]\n", program_name);
	printf("Options:\n");
	printf("  -c	Cycles (20 if not specified)\n");
	printf("  -v	Verbose mode\n");
	printf("  -l	Load state from program.bin.state at start\n");
	printf("  -s	Save state to program.bin.state at end\n");
//	printf("  -r	Randomize contents of memory\n");
//	printf("  -t	Time how long processing takes to complete\n");
	printf("Examples:\n");
	printf("  %s program.bin -c 200		Runs program.bin for 200 cycles\n", program_name);
	printf("  %s example.bin -s -v		Runs example.bin saving state, verbose\n", program_name);
}
