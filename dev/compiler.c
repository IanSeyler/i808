/*
compiler.c - Module file for i808 source code compiler
v1.0b - 2007.11.15
Coded by Ian Seyler (iseyler@gmail.com)
*/

/* Obligatory includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Version information strings */
char VERSIONINFO1[] = "i808 Compiler v1.0b";
char VERSIONINFO2[] = "Compiled on 2007.11.15, based on i808 spec 2006.08.30";
/* Function prototypes */
int hex2dec(char val);
int incCounter();
int incPageCounter();
void printhelp();

/* Global variables */
char arg[50], parsedarg[20];
unsigned int writeCounter = 0, writePageCounter = 0;
unsigned int argval = 0, argvalue = 0;
int verbose = 0, debug = 0, error = 0;
int maxProgramSize = 0, maxPages = 0;
int destArch = 0; // i808 = 8080, i808v2 = 8081, i816 = 8160, i816v2 = i8161, 0 = not specified
char *program_name;

int main(int argc, char *argv[])
{
	char *inputFile, outputFile[30];
	int tint = 0, tint2 = 0;
    char VAR[] = "var";
	char NOP[] = "nop";
	char ADD[] = "add";
	char SUB[] = "sub";
	char MOV[] = "mov";
	char CMP[] = "cmp";
	char INC[] = "inc";
	char DEC[] = "dec";
	char AND[] = "and";
	char OR[] = "or";
	char XOR[] = "xor";
	char NOT[] = "not";
	char IN[] = "in";
	char OUT[] = "out";
	char PUSH[] = "push";
	char POP[] = "pop";
	char JL[] = "jl";
	char JLE[] = "jle";
	char JE[] = "je";
	char JGE[] = "jge";
	char JG[] = "jg";
	char JNE[] = "jne";
	char JMP[] = "jmp";
	char JSF[] = "jsf";
	char JSB[] = "jsb";
	char CALL[] = "call";
	char RET[] = "ret";
	char BSL[] = "bsl";
	char BSR[] = "bsr";
	char PUSHA[] = "pusha";
	char POPA[] = "popa";
	char RND[] = "rnd";
	int IP = 0, memval1 = 0, memval2 = 0;
	unsigned int argval1 = 0, argval2 = 0, argvalue1 = 0, argvalue2 = 0;
	int subOptionPointer[50], subOptionRunningNumber = 0;
	int tempJumpAddress = 0;
	char subOptionName[50][20]; // 20 chars each
	char tempsubOptionName[50];
	unsigned char jumpcounter[50], jumptint = 0;
    unsigned int varCounter = 0;
    char varName[50][20];
    unsigned int varAddress[50];
    unsigned int varRunningAddress = 255;
	char parsed[50], arg1[50], arg2[50], parsedarg1[20], parsedarg2[20];
	unsigned char compiledCode[2][256];
//	unsigned char compiled16bitCode[2][65536];
	unsigned char tchar;
	FILE *fpIn, *fpOut;

	program_name = argv[0];

	if (argc < 2 || argv[1][0] == '-' || argv[1][0] == '/') // make sure there is at least one argument (filename)
	{
		printhelp();
		exit (1);
	}

	inputFile = argv[1];
//	printf("Start\n");
	for(tint=0; tint<strlen(inputFile); tint++)
	{
		if (inputFile[tint] == '.')
		{
			outputFile[tint] = '.'; // Could have used strcat
			outputFile[tint+1] = 'b';
			outputFile[tint+2] = 'i';
			outputFile[tint+3] = 'n';
			outputFile[tint+4] = '\0';
			break;
		}
		else
		{
			outputFile[tint] = inputFile[tint];
		}
	}
	
	// Parse the arguments
	for(tint=1; tint<argc; tint++)
	{
		if (argv[tint][0] == '-' || argv[tint][0] == '/')
		{
			if (argv[tint][1] == 'v') //check for verbose mode
			{
				verbose = 1;
			}
			else if (argv[tint][1] == 'd') // check for debug mode
			{
				debug = 1;
			}
			else if (argv[tint][1] == '?') // check for help
			{
				printhelp();
				exit(1);
			}
		}
	}

	if ((fpIn = fopen(inputFile, "r")) == NULL)
	{
		printf("\nCompile Problem:\nError opening source file %s\n", inputFile);
		exit (1);
	}

	if (verbose == 1) printf("%s\n%s\n\n", VERSIONINFO1, VERSIONINFO2);

	if (verbose == 1) printf("Initial setup...\n");

	// Check for architecture
	if (verbose == 1) printf("Checking for destination arch... ");
	while (!feof(fpIn))
	{
		fscanf(fpIn, "%s", &parsed);
		if (parsed[0] == 'd' && parsed[1] == 'e' && parsed[2] == 's' && parsed[3] == 't' && parsed[4] == '=')	// Suboption
		{
		//	printf("Found a dest line : ""%s""\n", parsed);
			tint = strlen(parsed);
			if (parsed[6] == '8' && parsed[7] == '0' && parsed[8] == '8')
			{
				if (parsed[9] == 'v' && parsed[10] == '2')
				{
					if (verbose == 1) printf("i808v2\n");
					destArch = 8081;
					maxProgramSize = 512;
					maxPages = 2;
				}
				else
				{
					if (verbose == 1) printf("i808\n");
					destArch = 8080;
					maxProgramSize = 256;
					maxPages = 1;
				}
			}
			else if (parsed[6] == '8' && parsed[7] == '1' && parsed[8] == '6')
			{
				if (parsed[9] == 'v' && parsed[10] == '2')
				{
					if (verbose == 1) printf("i816v2\n");
					destArch = 8161;
					maxProgramSize = 131072;
					maxPages = 2;
				}
				else
				{
					if (verbose == 1) printf("i816\n");
					destArch = 8160;
					maxProgramSize = 65536;
					maxPages = 1;
				}
			}
			else
			{
				if (verbose == 1) printf("invalid selection.. defaulting to i808\n");
				destArch = 8080;
				maxProgramSize = 256;
			}
		}
	}
	if (destArch == 0)
	{
		if (verbose == 1)
			printf("not found (defaulting to i808)\n");
		else
			printf("Warning: Destination architecture not specified (defaulting to i808)\n");
		destArch = 8080;
		maxProgramSize = 256;
	}
	if (verbose == 1) printf("\n");

	rewind(fpIn);

	for (tint=0; tint<2; tint++)
	{
		for (tint2=0; tint2<256; tint2++)
		{
			compiledCode[tint][tint2] = 0x00;
		}
	}

	if (verbose == 1) printf("Compiler Stage 1 of 3 (Jumps)...\n");

	//Get the suboption names
	if (verbose == 1) printf("The following suboptions were detected...\n");
	while (!feof(fpIn))
	{
		fscanf(fpIn, "%s", &parsed);
		if (parsed[0] == ':')																// Suboption
		{
			for (tint=1; tint<20; tint++) //Get just the name.
				subOptionName[subOptionRunningNumber][tint-1] = parsed[tint]; // string termination???
				
			if (verbose == 1) printf("- %s \n", subOptionName[subOptionRunningNumber]);
			subOptionRunningNumber++;
		}	
	}
	rewind(fpIn);

	if (verbose == 1) printf("\n");
	
	if (verbose == 1) printf("Compiler Stage 2 of 3 (Encoding)...\n");

	// First pass at the compile
//	while (fscanf(fpIn, "%s", &parsed) != NULL)
	while (!feof(fpIn))
	{
		fscanf(fpIn, "%s", &parsed);
		if (feof(fpIn) != 0) // Needed because feof isn't accurate
			break;

//		printf("%s ", parsed);
        if (strcmp(parsed, VAR) == 0)
        {
            int keepgoing = 1;
            do {
                fscanf(fpIn, "%s", &parsed);
                strcpy(varName[varCounter], parsed);
//                printf("%s", varName[varCounter]);
                varCounter++;
                for (tint=0; tint<strlen(parsed); tint++)
                    if (parsed[tint] == ';') // The last variable
                    {
                        varName[varCounter-1][strlen(parsed) - 1] = '\0';
                        keepgoing = 0;
                    }
            } while(keepgoing == 1);
            if (verbose == 1) printf("Registered %d variables:\n", varCounter);
            for(tint=0; tint<varCounter; tint++)
            {
                varAddress[tint] = varRunningAddress;
                varRunningAddress--;
                if (verbose == 1) printf("%s @ 0x%02X\n", varName[tint], varAddress[tint]);
            }
        }
		
		else if (strcmp(parsed, NOP) == 0)														// NOP
		{
			if (verbose == 1) printf("NOP\n");
			IP++;
			compiledCode[writePageCounter][writeCounter] = 0x00;
			incCounter();
		}
		else if (strcmp(parsed, RET) == 0)														// RET
		{
			if (verbose == 1) printf("RET\n");
			IP++;
			compiledCode[writePageCounter][writeCounter] = 0x01;
			incCounter();
		}
		else if (strcmp(parsed, PUSHA) == 0)													// PUSHA
		{
			if (verbose == 1) printf("PUSHA\n");
			IP++;
			compiledCode[writePageCounter][writeCounter] = 0x02;
			incCounter();
		}
		else if (strcmp(parsed, POPA) == 0)														// POPA
		{
			if (verbose == 1) printf("POPA\n");
			IP++;
			compiledCode[writePageCounter][writeCounter] = 0x03;
			incCounter();
		}
		else if ((strcmp(parsed, ADD) == 0) || (strcmp(parsed, SUB) == 0) || (strcmp(parsed, MOV) == 0) || (strcmp(parsed, CMP) == 0) || (strcmp(parsed, AND) == 0) || (strcmp(parsed, OR) == 0) || (strcmp(parsed, XOR) == 0) || (strcmp(parsed, IN) == 0) || (strcmp(parsed, OUT) == 0))
		{
			fscanf(fpIn, "%s", &arg1);
			fscanf(fpIn, "%s", &arg2);

//			argval1 = parseArguement(&arg1);

			// Parse first argument
			if (arg1[0] == 'A')
				argval1 = 0;
			else if (arg1[0] == 'B')
				argval1 = 1;
			else if (arg1[0] == 'C')
				argval1 = 2;
			else if (arg1[0] == 'D')
				argval1 = 3;
			else if (arg1[0] == '[' && arg1[1] == 'A')
				argval1 = 4;
			else if (arg1[0] == '[' && arg1[1] == 'B')
				argval1 = 5;
			else if (arg1[0] == '[' && arg1[1] == 'C')
				argval1 = 6;
			else if (arg1[0] == '[' && arg1[1] == 'D')
				argval1 = 7;
			else if (isdigit(arg1[0]))
			{
				argval1 = 8;
				if (arg1[1] == 'x' || arg1[1] == 'X')
				{
					argvalue1 = (hex2dec(arg1[2]) * 16) + hex2dec(arg1[3]);
				}
				else
				{
					parsedarg1[0] = arg1[0];
					parsedarg1[1] = arg1[1];
					parsedarg1[2] = arg1[2];
					argvalue1 = atoi(parsedarg1);
				}
			}
			else if (arg1[0] == '[' && isdigit(arg1[1]))
			{
				argval1 = 9;
				if (arg1[2] == 'x' || arg1[2] == 'X')
				{
					argvalue1 = (hex2dec(arg1[3]) * 16) + hex2dec(arg1[4]);
				}
				else
				{
					parsedarg1[0] = arg1[1];
					parsedarg1[1] = arg1[2];
					parsedarg1[2] = arg1[3];
					argvalue1 = atoi(parsedarg1);
				}
			}
			else
			{
				if (verbose == 1) printf("First argument error and ");
				error++;
				argval1 = 0;
			}

			// Parse second argument
			if (arg2[0] == 'A')
				argval2 = 0;
			else if (arg2[0] == 'B')
				argval2 = 1;
			else if (arg2[0] == 'C')
				argval2 = 2;
			else if (arg2[0] == 'D')
				argval2 = 3;
			else if (arg2[0] == '[' && arg2[1] == 'A')
				argval2 = 4;
			else if (arg2[0] == '[' && arg2[1] == 'B')
				argval2 = 5;
			else if (arg2[0] == '[' && arg2[1] == 'C')
				argval2 = 6;
			else if (arg2[0] == '[' && arg2[1] == 'D')
				argval2 = 7;
			else if (isdigit(arg2[0]))
			{
				argval2 = 8;
				if (arg2[1] == 'x' || arg2[1] == 'X')
				{
					argvalue2 = (hex2dec(arg2[2]) * 16) + hex2dec(arg2[3]);
				}
				else
				{
					parsedarg2[0] = arg2[0];
					parsedarg2[1] = arg2[1];
					parsedarg2[2] = arg2[2];
					argvalue2 = atoi(parsedarg2);
				}
			}
			else if (arg2[0] == '[' && isdigit(arg2[1]))
			{
				argval2 = 9;
				if (arg2[2] == 'x' || arg2[2] == 'X')
				{
					argvalue2 = (hex2dec(arg2[3]) * 16) + hex2dec(arg2[4]);
				}
				else
				{
					parsedarg2[0] = arg2[1];
					parsedarg2[1] = arg2[2];
					parsedarg2[2] = arg2[3];
					argvalue2 = atoi(parsedarg2);
				}
			}
			else
			{
				if (verbose == 1) printf("Second argument error and ");
				error++;
				argval2 = 0;
			}

			if (strcmp(parsed, ADD) == 0)
			{
				if (argval1 == 0 && argval2 == 0)
				{
					if (verbose == 1) printf("ADD A, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x10;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 1)
				{
					if (verbose == 1) printf("ADD A, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x11;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 2)
				{
					if (verbose == 1) printf("ADD A, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x12;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 3)
				{
					if (verbose == 1) printf("ADD A, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x13;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 8)
				{
					if (verbose == 1) printf("ADD A, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x90;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 0)
				{
					if (verbose == 1) printf("ADD B, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x14;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 1)
				{
					if (verbose == 1) printf("ADD B, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x15;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 2)
				{
					if (verbose == 1) printf("ADD B, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x16;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 3)
				{
					if (verbose == 1) printf("ADD B, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x17;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 8)
				{
					if (verbose == 1) printf("ADD B, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x91;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 0)
				{
					if (verbose == 1) printf("ADD C, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x18;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 1)
				{
					if (verbose == 1) printf("ADD C, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x19;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 2)
				{
					if (verbose == 1) printf("ADD C, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x1A;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 3)
				{
					if (verbose == 1) printf("ADD C, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x1B;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 8)
				{
					if (verbose == 1) printf("ADD C, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x92;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 0)
				{
					if (verbose == 1) printf("ADD D, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x1C;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 1)
				{
					if (verbose == 1) printf("ADD D, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x1D;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 2)
				{
					if (verbose == 1) printf("ADD D, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x1E;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 3)
				{
					if (verbose == 1) printf("ADD D, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x1F;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 8)
				{
					if (verbose == 1) printf("ADD D, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x93;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("ADD Error!");
					error++;
				}
			}
			else if (strcmp(parsed, SUB) == 0)
			{
				if (argval1 == 0 && argval2 == 0)
				{
					if (verbose == 1) printf("SUB A, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x20;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 1)
				{
					if (verbose == 1) printf("SUB A, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x21;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 2)
				{
					if (verbose == 1) printf("SUB A, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x22;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 3)
				{
					if (verbose == 1) printf("SUB A, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x23;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 8)
				{
					if (verbose == 1) printf("SUB A, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x94;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 0)
				{
					if (verbose == 1) printf("SUB B, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x24;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 1)
				{
					if (verbose == 1) printf("SUB B, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x25;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 2)
				{
					if (verbose == 1) printf("SUB B, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x26;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 3)
				{
					if (verbose == 1) printf("SUB B, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x27;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 8)
				{
					if (verbose == 1) printf("SUB B, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x95;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 0)
				{
					if (verbose == 1) printf("SUB C, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x28;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 1)
				{
					if (verbose == 1) printf("SUB C, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x29;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 2)
				{
					if (verbose == 1) printf("SUB C, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x2A;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 3)
				{
					if (verbose == 1) printf("SUB C, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x2B;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 8)
				{
					if (verbose == 1) printf("SUB C, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x96;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 0)
				{
					if (verbose == 1) printf("SUB D, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x2C;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 1)
				{
					if (verbose == 1) printf("SUB D, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x2D;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 2)
				{
					if (verbose == 1) printf("SUB D, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x2E;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 3)
				{
					if (verbose == 1) printf("SUB D, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x2F;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 8)
				{
					if (verbose == 1) printf("SUB D, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x97;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("SUB Error!");
					error++;
				}
			}
			else if (strcmp(parsed, MOV) == 0)
			{
				if (argval1 == 0 && argval2 == 0)
				{
					if (verbose == 1) printf("MOV A, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x30;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 1)
				{
					if (verbose == 1) printf("MOV A, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x31;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 2)
				{
					if (verbose == 1) printf("MOV A, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x32;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 3)
				{
					if (verbose == 1) printf("MOV A, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x33;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 8)
				{
					if (verbose == 1) printf("MOV A, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x98;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 0)
				{
					if (verbose == 1) printf("MOV B, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x34;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 1)
				{
					if (verbose == 1) printf("MOV B, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x35;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 2)
				{
					if (verbose == 1) printf("MOV B, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x36;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 3)
				{
					if (verbose == 1) printf("MOV B, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x37;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 8)
				{
					if (verbose == 1) printf("MOV B, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x99;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 0)
				{
					if (verbose == 1) printf("MOV C, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x38;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 1)
				{
					if (verbose == 1) printf("MOV C, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x39;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 2)
				{
					if (verbose == 1) printf("MOV C, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x3A;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 3)
				{
					if (verbose == 1) printf("MOV C, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x3B;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 8)
				{
					if (verbose == 1) printf("MOV C, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x9A;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 0)
				{
					if (verbose == 1) printf("MOV D, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x3C;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 1)
				{
					if (verbose == 1) printf("MOV D, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x3D;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 2)
				{
					if (verbose == 1) printf("MOV D, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x3E;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 3)
				{
					if (verbose == 1) printf("MOV D, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x3F;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 8)
				{
					if (verbose == 1) printf("MOV D, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x9B;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 4 && argval2 == 0)
				{
					if (verbose == 1) printf("MOV [A], A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA0;
					incCounter();
				}
				else if (argval1 == 4 && argval2 == 1)
				{
					if (verbose == 1) printf("MOV [A], B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA1;
					incCounter();
				}
				else if (argval1 == 4 && argval2 == 2)
				{
					if (verbose == 1) printf("MOV [A], C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA2;
					incCounter();
				}
				else if (argval1 == 4 && argval2 == 3)
				{
					if (verbose == 1) printf("MOV [A], D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA3;
					incCounter();
				}
				else if (argval1 == 5 && argval2 == 0)
				{
					if (verbose == 1) printf("MOV [B], A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA4;
					incCounter();
				}
				else if (argval1 == 5 && argval2 == 1)
				{
					if (verbose == 1) printf("MOV [B], B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA5;
					incCounter();
				}
				else if (argval1 == 5 && argval2 == 2)
				{
					if (verbose == 1) printf("MOV [B], C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA6;
					incCounter();
				}
				else if (argval1 == 5 && argval2 == 3)
				{
					if (verbose == 1) printf("MOV [B], D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA7;
					incCounter();
				}
				else if (argval1 == 6 && argval2 == 0)
				{
					if (verbose == 1) printf("MOV [C], A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA8;
					incCounter();
				}
				else if (argval1 == 6 && argval2 == 1)
				{
					if (verbose == 1) printf("MOV [C], B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xA9;
					incCounter();
				}
				else if (argval1 == 6 && argval2 == 2)
				{
					if (verbose == 1) printf("MOV [C], C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xAA;
					incCounter();
				}
				else if (argval1 == 6 && argval2 == 3)
				{
					if (verbose == 1) printf("MOV [C], D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xAB;
					incCounter();
				}
				else if (argval1 == 7 && argval2 == 0)
				{
					if (verbose == 1) printf("MOV [D], A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xAC;
					incCounter();
				}
				else if (argval1 == 7 && argval2 == 1)
				{
					if (verbose == 1) printf("MOV [D], B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xAD;
					incCounter();
				}
				else if (argval1 == 7 && argval2 == 2)
				{
					if (verbose == 1) printf("MOV [D], C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xAE;
					incCounter();
				}
				else if (argval1 == 7 && argval2 == 3)
				{
					if (verbose == 1) printf("MOV [D], D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xAF;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 4)
				{
					if (verbose == 1) printf("MOV A, [A]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB0;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 5)
				{
					if (verbose == 1) printf("MOV A, [B]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB1;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 6)
				{
					if (verbose == 1) printf("MOV A, [C]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB2;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 7)
				{
					if (verbose == 1) printf("MOV A, [D]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB3;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 4)
				{
					if (verbose == 1) printf("MOV B, [A]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB4;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 5)
				{
					if (verbose == 1) printf("MOV B, [B]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB5;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 6)
				{
					if (verbose == 1) printf("MOV B, [C]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB6;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 7)
				{
					if (verbose == 1) printf("MOV B, [D]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB7;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 4)
				{
					if (verbose == 1) printf("MOV C, [A]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB8;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 5)
				{
					if (verbose == 1) printf("MOV C, [B]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xB9;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 6)
				{
					if (verbose == 1) printf("MOV C, [C]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xBA;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 7)
				{
					if (verbose == 1) printf("MOV C, [D]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xBB;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 4)
				{
					if (verbose == 1) printf("MOV D, [A]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xBC;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 5)
				{
					if (verbose == 1) printf("MOV D, [B]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xBD;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 6)
				{
					if (verbose == 1) printf("MOV D, [C]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xBE;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 7)
				{
					if (verbose == 1) printf("MOV D, [D]");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xBF;
					incCounter();
				}
				else if (argval1 == 9 && argval2 == 0)
				{
					if (verbose == 1) printf("MOV [mem], A");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x88;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue1;
					incCounter();
				}
				else if (argval1 == 9 && argval2 == 1)
				{
					if (verbose == 1) printf("MOV [mem], B");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x89;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue1;
					incCounter();
				}
				else if (argval1 == 9 && argval2 == 2)
				{
					if (verbose == 1) printf("MOV [mem], C");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x8A;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue1;
					incCounter();
				}
				else if (argval1 == 9 && argval2 == 3)
				{
					if (verbose == 1) printf("MOV [mem], D");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x8B;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue1;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 9)
				{
					if (verbose == 1) printf("MOV A, [mem]");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x8C;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 9)
				{
					if (verbose == 1) printf("MOV B, [mem]");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x8D;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 9)
				{
					if (verbose == 1) printf("MOV C, [mem]");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x8E;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 9)
				{
					if (verbose == 1) printf("MOV D, [mem]");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x8F;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("MOV Error!");
					error++;
				}				
			}
			else if (strcmp(parsed, CMP) == 0)
			{
				if (argval1 == 0 && argval2 == 0)
				{
					if (verbose == 1) printf("CMP A, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x40;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 1)
				{
					if (verbose == 1) printf("CMP A, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x41;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 2)
				{
					if (verbose == 1) printf("CMP A, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x42;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 3)
				{
					if (verbose == 1) printf("CMP A, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x43;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 8)
				{
					if (verbose == 1) printf("CMP A, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x9C;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 0)
				{
					if (verbose == 1) printf("CMP B, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x44;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 1)
				{
					if (verbose == 1) printf("CMP B, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x45;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 2)
				{
					if (verbose == 1) printf("CMP B, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x46;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 3)
				{
					if (verbose == 1) printf("CMP B, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x47;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 8)
				{
					if (verbose == 1) printf("CMP B, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x9D;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 0)
				{
					if (verbose == 1) printf("CMP C, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x48;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 1)
				{
					if (verbose == 1) printf("CMP C, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x49;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 2)
				{
					if (verbose == 1) printf("CMP C, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x4A;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 3)
				{
					if (verbose == 1) printf("CMP C, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x4B;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 8)
				{
					if (verbose == 1) printf("CMP C, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x9E;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 0)
				{
					if (verbose == 1) printf("CMP D, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x4C;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 1)
				{
					if (verbose == 1) printf("CMP D, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x4D;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 2)
				{
					if (verbose == 1) printf("CMP D, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x4E;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 3)
				{
					if (verbose == 1) printf("CMP D, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x4F;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 8)
				{
					if (verbose == 1) printf("CMP D, val");
					IP+=2;
					compiledCode[writePageCounter][writeCounter] = 0x9F;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = argvalue2;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("CMP Error!");
					error++;
				}
			}
			else if (strcmp(parsed, AND) == 0)
			{
				if (argval1 == 0 && argval2 == 0)
				{
					if (verbose == 1) printf("AND A, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x50;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 1)
				{
					if (verbose == 1) printf("AND A, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x51;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 2)
				{
					if (verbose == 1) printf("AND A, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x52;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 3)
				{
					if (verbose == 1) printf("AND A, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x53;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 0)
				{
					if (verbose == 1) printf("AND B, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x54;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 1)
				{
					if (verbose == 1) printf("AND B, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x55;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 2)
				{
					if (verbose == 1) printf("AND B, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x56;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 3)
				{
					if (verbose == 1) printf("AND B, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x57;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 0)
				{
					if (verbose == 1) printf("AND C, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x58;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 1)
				{
					if (verbose == 1) printf("AND C, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x59;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 2)
				{
					if (verbose == 1) printf("AND C, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x5A;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 3)
				{
					if (verbose == 1) printf("AND C, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x5B;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 0)
				{
					if (verbose == 1) printf("AND D, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x5C;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 1)
				{
					if (verbose == 1) printf("AND D, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x5D;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 2)
				{
					if (verbose == 1) printf("AND D, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x5E;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 3)
				{
					if (verbose == 1) printf("AND D, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x5F;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("AND Error!");
					error++;
				}
			}
			else if (strcmp(parsed, OR) == 0)
			{
				if (argval1 == 0 && argval2 == 0)
				{
					if (verbose == 1) printf("OR A, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x60;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 1)
				{
					if (verbose == 1) printf("OR A, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x61;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 2)
				{
					if (verbose == 1) printf("OR A, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x62;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 3)
				{
					if (verbose == 1) printf("OR A, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x63;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 0)
				{
					if (verbose == 1) printf("OR B, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x64;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 1)
				{
					if (verbose == 1) printf("OR B, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x65;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 2)
				{
					if (verbose == 1) printf("OR B, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x66;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 3)
				{
					if (verbose == 1) printf("OR B, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x67;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 0)
				{
					if (verbose == 1) printf("OR C, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x68;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 1)
				{
					if (verbose == 1) printf("OR C, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x69;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 2)
				{
					if (verbose == 1) printf("OR C, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x6A;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 3)
				{
					if (verbose == 1) printf("OR C, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x6B;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 0)
				{
					if (verbose == 1) printf("OR D, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x6C;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 1)
				{
					if (verbose == 1) printf("OR D, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x6D;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 2)
				{
					if (verbose == 1) printf("OR D, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x6E;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 3)
				{
					if (verbose == 1) printf("OR D, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x6F;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("OR Error!");
					error++;
				}
			}
			else if (strcmp(parsed, XOR) == 0)
			{
				if (destArch != 8080)
				{
					printf("Error! The XOR command is not supported with the selected destination architecture.\n");
				}
				if (argval1 == 0 && argval2 == 0)
				{
					if (verbose == 1) printf("XOR A, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x70;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 1)
				{
					if (verbose == 1) printf("XOR A, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x71;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 2)
				{
					if (verbose == 1) printf("XOR A, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x72;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 3)
				{
					if (verbose == 1) printf("XOR A, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x73;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 0)
				{
					if (verbose == 1) printf("XOR B, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x74;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 1)
				{
					if (verbose == 1) printf("XOR B, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x75;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 2)
				{
					if (verbose == 1) printf("XOR B, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x76;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 3)
				{
					if (verbose == 1) printf("XOR B, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x77;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 0)
				{
					if (verbose == 1) printf("XOR C, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x78;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 1)
				{
					if (verbose == 1) printf("XOR C, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x79;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 2)
				{
					if (verbose == 1) printf("XOR C, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x7A;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 3)
				{
					if (verbose == 1) printf("XOR C, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x7B;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 0)
				{
					if (verbose == 1) printf("XOR D, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x7C;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 1)
				{
					if (verbose == 1) printf("XOR D, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x7D;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 2)
				{
					if (verbose == 1) printf("XOR D, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x7E;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 3)
				{
					if (verbose == 1) printf("XOR D, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x7F;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("XOR Error!");
					error++;
				}
			}
			else if (strcmp(parsed, IN) == 0)
			{
				if (argval1 == 0 && argval2 == 0)
				{
					if (verbose == 1) printf("IN A, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC0;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 1)
				{
					if (verbose == 1) printf("IN A, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC1;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 2)
				{
					if (verbose == 1) printf("IN A, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC2;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 3)
				{
					if (verbose == 1) printf("IN A, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC3;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 0)
				{
					if (verbose == 1) printf("IN B, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC4;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 1)
				{
					if (verbose == 1) printf("IN B, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC5;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 2)
				{
					if (verbose == 1) printf("IN B, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC6;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 3)
				{
					if (verbose == 1) printf("IN B, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC7;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 0)
				{
					if (verbose == 1) printf("IN C, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC8;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 1)
				{
					if (verbose == 1) printf("IN C, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xC9;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 2)
				{
					if (verbose == 1) printf("IN C, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xCA;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 3)
				{
					if (verbose == 1) printf("IN C, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xCB;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 0)
				{
					if (verbose == 1) printf("IN D, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xCC;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 1)
				{
					if (verbose == 1) printf("IN D, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xCD;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 2)
				{
					if (verbose == 1) printf("IN D, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xCE;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 3)
				{
					if (verbose == 1) printf("IN D, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xCF;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("IN Error!");
					error++;
				}
			}
			else if (strcmp(parsed, OUT) == 0)
			{
				if (argval1 == 0 && argval2 == 0)
				{
					if (verbose == 1) printf("OUT A, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD0;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 1)
				{
					if (verbose == 1) printf("OUT A, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD1;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 2)
				{
					if (verbose == 1) printf("OUT A, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD2;
					incCounter();
				}
				else if (argval1 == 0 && argval2 == 3)
				{
					if (verbose == 1) printf("OUT A, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD3;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 0)
				{
					if (verbose == 1) printf("OUT B, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD4;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 1)
				{
					if (verbose == 1) printf("OUT B, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD5;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 2)
				{
					if (verbose == 1) printf("OUT B, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD6;
					incCounter();
				}
				else if (argval1 == 1 && argval2 == 3)
				{
					if (verbose == 1) printf("OUT B, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD7;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 0)
				{
					if (verbose == 1) printf("OUT C, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD8;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 1)
				{
					if (verbose == 1) printf("OUT C, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xD9;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 2)
				{
					if (verbose == 1) printf("OUT C, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xDA;
					incCounter();
				}
				else if (argval1 == 2 && argval2 == 3)
				{
					if (verbose == 1) printf("OUT C, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xDB;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 0)
				{
					if (verbose == 1) printf("OUT D, A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xDC;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 1)
				{
					if (verbose == 1) printf("OUT D, B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xDD;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 2)
				{
					if (verbose == 1) printf("OUT D, C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xDE;
					incCounter();
				}
				else if (argval1 == 3 && argval2 == 3)
				{
					if (verbose == 1) printf("OUT D, D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xDF;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("OUT Error!");
					error++;
				}
			}
			else
			{
				if (verbose == 1) printf("WTF? How could you get here???");
				error+=1000000;
			}
			if (verbose == 1) printf("\n");
		}
		else if ((parsed[0] == 'j' && parsed[1] != 's') || (parsed[0] == 'c' && parsed[1] == 'a'))																// Jumps
		{
			fscanf(fpIn, "%s", &arg1);
			
			for (tint=0; tint<50; tint++)
			{
				if (strcmp(arg1, subOptionName[tint]) == 0)
					tempJumpAddress = tint;
//				else
//					printf("ADDRESS ERROR\n");
			}
			if (strcmp(parsed, JL) == 0)													// JL
			{
				if (verbose == 1) printf("JL ");
					compiledCode[writePageCounter][writeCounter] = 0x80;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = tempJumpAddress;
					incCounter();
			}
			else if (strcmp(parsed, JLE) == 0)													// JLE
			{
				if (verbose == 1) printf("JLE ");
					compiledCode[writePageCounter][writeCounter] = 0x81;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = tempJumpAddress;
					incCounter();
			}
			else if (strcmp(parsed, JE) == 0)													// JE
			{
				if (verbose == 1) printf("JE ");
					compiledCode[writePageCounter][writeCounter] = 0x82;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = tempJumpAddress;
					incCounter();
			}
			else if (strcmp(parsed, JGE) == 0)													// JGE
			{
				if (verbose == 1) printf("JGE ");
					compiledCode[writePageCounter][writeCounter] = 0x83;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = tempJumpAddress;
					incCounter();
			}
			else if (strcmp(parsed, JG) == 0)													// JG
			{
				if (verbose == 1) printf("JG ");
					compiledCode[writePageCounter][writeCounter] = 0x84;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = tempJumpAddress;
					incCounter();
			}
			else if (strcmp(parsed, JMP) == 0)														// JMP
			{
				if (verbose == 1) printf("JMP ");
					compiledCode[writePageCounter][writeCounter] = 0x85;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = tempJumpAddress;
					incCounter();
			}
			else if (strcmp(parsed, JNE) == 0)														// JMP
			{
				if (verbose == 1) printf("JNE ");
					compiledCode[writePageCounter][writeCounter] = 0x86;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = tempJumpAddress;
					incCounter();
			}
			else if (strcmp(parsed, CALL) == 0)														// JSF
			{
				if (verbose == 1) printf("CALL ");
					compiledCode[writePageCounter][writeCounter] = 0x87;
					incCounter();
					compiledCode[writePageCounter][writeCounter] = tempJumpAddress;
					incCounter();
			}
			else
			{
				if (verbose == 1) printf("JMP ERROR. Bad Jump to ");
				error++;
			}
			if (verbose == 1) printf("%s", arg1);
			IP+=2;
//			if (verbose == 1) printf("\n");
			jumpcounter[jumptint++] = IP-2;
			if (verbose == 1) printf(" (jump %d at byte 0x%02X)\n", jumptint, jumpcounter[jumptint-1]);
		}
		else if ((strcmp(parsed, INC) == 0) || (strcmp(parsed, DEC) == 0) || (strcmp(parsed, NOT) == 0) || (strcmp(parsed, POP) == 0) || (strcmp(parsed, PUSH) == 0) || (strcmp(parsed, BSL) == 0) || (strcmp(parsed, BSR) == 0) || (strcmp(parsed, JSF) == 0) || (strcmp(parsed, JSB) == 0) || (strcmp(parsed, RND) == 0))
		{
			fscanf(fpIn, "%s", &arg1);

			if (strcmp(parsed, INC) == 0)														// INC
			{
				if (verbose == 1) printf("INC ");
				if (arg1[0] == 'A')								// inc A -- 0x08
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x08;
					incCounter();
				}
				else if (arg1[0] == 'B')						// inc B -- 0x09
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x09;
					incCounter();
				}
				else if (arg1[0] == 'C')						// inc C -- 0x0A
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x0A;
					incCounter();
				}
				else if (arg1[0] == 'D')						// inc D -- 0x0B
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x0B;
					incCounter();
				}
				else																// compilation error
				{
					if (verbose == 1) printf("INC ERROR!");
					error++;
				}
			}
			else if (strcmp(parsed, DEC) == 0)														// DEC
			{
				if (verbose == 1) printf("DEC ");
				if (arg1[0] == 'A')								// dec A -- 0x0C
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x0C;
					incCounter();
				}
				else if (arg1[0] == 'B')						// dec B -- 0x0D
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x0D;
					incCounter();
				}
				else if (arg1[0] == 'C')						// dec C -- 0x0E
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x0E;
					incCounter();
				}
				else if (arg1[0] == 'D')						// dec D -- 0x0F
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x0F;
					incCounter();
				}
				else																// compilation error
				{
					if (verbose == 1) printf("DEC ERROR!");
					error++;
				}
			}
			else if (strcmp(parsed, NOT) == 0)														// NOT
			{
				if (verbose == 1) printf("NOT ");
				if (arg1[0] == 'A')								// not A -- 0x04
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x04;
					incCounter();
				}
				else if (arg1[0] == 'B')						// not B -- 0x05
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x05;
					incCounter();
				}
				else if (arg1[0] == 'C')						// not C -- 0x06
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x06;
					incCounter();
				}
				else if (arg1[0] == 'D')						// not D -- 0x07
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0x07;
					incCounter();
				}
				else																// compilation error
				{
					if (verbose == 1) printf("NOT ERROR!");
					error++;
				}
			}
			else if (strcmp(parsed, PUSH) == 0)														// PUSH
			{
				if (verbose == 1) printf("PUSH ");
				if (arg1[0] == 'A')
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE0;
					incCounter();
				}
				else if (arg1[0] == 'B')
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE1;
					incCounter();
				}
				else if (arg1[0] == 'C')
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE2;
					incCounter();
				}
				else if (arg1[0] == 'D')
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE3;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("PUSH ERROR!");
					error++;
				}
			}
			else if (strcmp(parsed, POP) == 0)														// POP
			{
				if (verbose == 1) printf("POP ");
				if (arg1[0] == 'A')
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE4;
					incCounter();
				}
				else if (arg1[0] == 'B')
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE5;
					incCounter();
				}
				else if (arg1[0] == 'C')
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE6;
					incCounter();
				}
				else if (arg1[0] == 'D')
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE7;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("POP ERROR!");
					error++;
				}
			}
			else if (strcmp(parsed, BSL) == 0)														// BSL
			{
				if (verbose == 1) printf("BSL ");
				if (arg1[0] == 'A')
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE8;
					incCounter();
				}
				else if (arg1[0] == 'B')
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xE9;
					incCounter();
				}
				else if (arg1[0] == 'C')
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xEA;
					incCounter();
				}
				else if (arg1[0] == 'D')
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xEB;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("BSL ERROR!");
					error++;
				}
			}
			else if (strcmp(parsed, BSR) == 0)														// BSR
			{
				if (verbose == 1) printf("BSR ");
				if (arg1[0] == 'A')
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xEC;
					incCounter();
				}
				else if (arg1[0] == 'B')
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xED;
					incCounter();
				}
				else if (arg1[0] == 'C')
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xEE;
					incCounter();
				}
				else if (arg1[0] == 'D')
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xEF;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("BSR ERROR!");
					error++;
				}
			}
			else if (strcmp(parsed, JSF) == 0)														// JSF
			{
				if (verbose == 1) printf("JSF ");
				if (arg1[0] == 'A')
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF0;
					incCounter();
				}
				else if (arg1[0] == 'B')
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF1;
					incCounter();
				}
				else if (arg1[0] == 'C')
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF2;
					incCounter();
				}
				else if (arg1[0] == 'D')
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF3;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("JSF ERROR!");
					error++;
				}
			}
			else if (strcmp(parsed, JSB) == 0)														// JSB
			{
				if (verbose == 1) printf("JSB ");
				if (arg1[0] == 'A')
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF4;
					incCounter();
				}
				else if (arg1[0] == 'B')
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF5;
					incCounter();
				}
				else if (arg1[0] == 'C')
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF6;
					incCounter();
				}
				else if (arg1[0] == 'D')
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF7;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("JSB ERROR!");
					error++;
				}
			}
			else if (strcmp(parsed, RND) == 0)														// JSB
			{
				if (verbose == 1) printf("RND ");
				if (arg1[0] == 'A')
				{
					if (verbose == 1) printf("A");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF8;
					incCounter();
				}
				else if (arg1[0] == 'B')
				{
					if (verbose == 1) printf("B");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xF9;
					incCounter();
				}
				else if (arg1[0] == 'C')
				{
					if (verbose == 1) printf("C");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xFA;
					incCounter();
				}
				else if (arg1[0] == 'D')
				{
					if (verbose == 1) printf("D");
					IP++;
					compiledCode[writePageCounter][writeCounter] = 0xFB;
					incCounter();
				}
				else
				{
					if (verbose == 1) printf("RND ERROR!");
					error++;
				}
			}
			if (verbose == 1) printf("\n");
		}
		else if (parsed[0] == ':')																// Suboption
		{
			if (verbose == 1) printf("Suboption ");

			for (tint=1; tint<20; tint++)
				tempsubOptionName[tint-1] = parsed[tint];
				
			if (verbose == 1) printf("'%s'", tempsubOptionName);
			
			for (tint=0; tint<50; tint++)
			{
				if (strcmp(tempsubOptionName, subOptionName[tint]) == 0)
				{
					subOptionPointer[tint] = IP;
					if (verbose == 1) printf(" at 0x%02X", IP);
				}
			}
			if (verbose == 1) printf("\n");
		}
		else if (parsed[0] == '/')																// Comment
		{
//			if (verbose == 1) printf("Comment, ");
			while (!feof(fpIn))
			{
				fscanf(fpIn, "%s", &parsed);
				if (parsed[0] == '/')
				{
//					if (verbose == 1) printf("End comment");
					break;
				}
			}
//			if (verbose == 1) printf("\n");
		}
		else if (parsed[0] == 'd' && parsed[1] == 'e' && parsed[2] == 's' && parsed[3] == 't')	// Destination
		{
			//do nothing, just ignore.
		}
		else if (parsed[0] == '@')																// Hard jump
		{
			tint = strlen(parsed);
			if (parsed[2] == 'x' || parsed[2] == 'X') // no error checking at all.. needs 16bit too!!
			{
				IP = (hex2dec(parsed[3]) * 16) + hex2dec(parsed[4]);
				writeCounter = IP;
			}
			else
			{
				parsedarg1[0] = parsed[1];
				parsedarg1[1] = parsed[2];
				parsedarg1[2] = parsed[3];
				IP = atoi(parsedarg1);
				writeCounter = IP;
			}
//			printf("Jumping to %d\n", IP);
		}
		else if (parsed[0] == 'D' && parsed[1] == 'A' && parsed[2] == 'T' && parsed[3] == 'A')	// Literal
		{
			for (tint=0; tint<10; tint++) // fix this!
			{
				tchar = getc(fpIn);
				if (tchar == '"')
				{
					while(1)
					{
						tchar = getc(fpIn); // This messes up the next scanf
						if (tchar == '"')
						{
							IP++;
							compiledCode[writePageCounter][writeCounter] = 0x00;
							incCounter();
							break;
						}
						else
						{
							IP++;
							compiledCode[writePageCounter][writeCounter] = tchar;
							incCounter();
						}
					}
				}
			}
		}
		else																					// Compilation Error
		{
			printf("Compiler Error. Unknown keyword '%s'\n", parsed);
			error++;
		}			
	} //end while		
	if (verbose == 1) printf("\n");
	
//	for (tint=0; tint<5; tint++)
//	{
//		printf("%s at %d\n", subOptionName[tint], subOptionPointer[tint]);
//	}
	
	fclose(fpIn);
	//Done first stage of compiling...


	if (verbose == 1) printf("Compiler Stage 3 of 3 (Transribing)...\n");

	for (tint=0; tint<maxProgramSize; tint++)
	{
		if (compiledCode[0][tint] >= 0x88 && compiledCode[0][tint] <= 0x9F) //Fix the jumps to proper address, 2-byte commands
		{
			tint++;
		}
		else if (compiledCode[0][tint] >= 0x80 && compiledCode[0][tint] <= 0x87) //Fix the jumps to proper address
		{
			if (verbose == 1) printf("0x%02X to ", compiledCode[0][tint]);
			tint++;
			tchar = compiledCode[0][tint];
			if (tchar > subOptionRunningNumber) printf("Potential jump error");
			tchar = subOptionPointer[tchar];
			if (verbose == 1) printf("0x%02X\n", tchar);
			compiledCode[0][tint] = tchar;
		}
	}

// jumptint-1, jumpcounter[jumptint-1]);
	
	fpOut = fopen(outputFile, "wb");
	fwrite(compiledCode[0], maxProgramSize, 1, fpOut);
	fclose(fpOut);
	
//	printf("Finish\n");
	if (verbose == 1) printf("\n");
	if (error == 0)
	{
		if (verbose == 1) printf("Compiled sucessfully. Saved %d bytes of code to %s\n", IP, outputFile);
	}
	else if (error == 1)
	{
		printf("There was an error during compile. Output may not be as expected.\n");
	}
	else
	{
		printf("There were %d errors during compile. Output may not be as expected.\n", error);
	}

//	system("PAUSE");
	return 0;
}

int hex2dec(char val)
{
	int temp;
/*	if (isdigit(val))
	{
		printf("Got a digit");
		temp = atoi(val);
	}*/
	if (val == '0')
	{
		temp = 0;
	}
	else if (val == '1')
	{
		temp = 1;
	}
	else if (val == '2')
	{
		temp = 2;
	}
	else if (val == '3')
	{
		temp = 3;
	}
	else if (val == '4')
	{
		temp = 4;
	}
	else if (val == '5')
	{
		temp = 5;
	}
	else if (val == '6')
	{
		temp = 6;
	}
	else if (val == '7')
	{
		temp = 7;
	}
	else if (val == '8')
	{
		temp = 8;
	}
	else if (val == '9')
	{
		temp = 9;
	}
	else if (val == 'a' || val == 'A')
	{
		temp = 10;
	}
	else if (val == 'b' || val == 'B')
	{
		temp = 11;
	}
	else if (val == 'c' || val == 'C')
	{
		temp = 12;
	}
	else if (val == 'd' || val == 'D')
	{
		temp = 13;
	}
	else if (val == 'e' || val == 'E')
	{
		temp = 14;
	}
	else if (val == 'f' || val == 'F')
	{
		temp = 15;
	}
	else
	{
		printf("Error during hex to dec conversion");
		temp = -1;
	}
	return temp;
}

int incCounter()
{
	writeCounter++;
	if (writeCounter > maxProgramSize)
		printf("Error! Compiled program has exceeded resources\n");
}

int incPageCounter()
{
	writePageCounter++;
	if (writePageCounter > maxPages)
		printf("Error! Compiled program has exceeded resources\n");
}

void printhelp()
{
	printf("%s\n", VERSIONINFO1);
	printf("%s\n", VERSIONINFO2);
	printf("Coded by Ian Seyler (iseyler@gmail.com)\n\n");
	printf("Usage: %s [source file] [options]\n", program_name);
	printf("Options:\n");
	printf("  -v	Verbose mode\n");
}
