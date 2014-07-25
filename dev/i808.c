/*
i808.c - Module file for i808 Virtual Computer
v1.0b - 2007.11.15
Coded by Ian Seyler (iseyler@gmail.com)
*/

/* Obligatory includes */
#include <stdio.h>
#include "i808.h"

/* Global variables */
unsigned char bit8fieldl[16] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
unsigned char bit8fieldh[16] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};

/* Version information strings */
char i808VERSIONINFO[] = "i808 Virtual Computer Library";
char i808VERSIONNUMINFO[] = "v1.0b (Standard) FOR PUBLIC DISTRIBUTION";
char i808VERSIONSUBINFO[] = "Designed and programmed by Ian Seyler (iseyler@gmail.com)";


/*========================================================*
 * Function: i808compute()
 * Purpose : Process an 808 computer
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 int cycles = Cycles to process
 *			 int verbose = 1 for verbose mode
 * Returns : nothing
 *========================================================*/

void i808compute(eightbitComputer *c, unsigned int cycles, unsigned int verbose)
{
	unsigned int step = 0, tint = 0;
	unsigned char temp = 0x00; // only being used to show zero and carry data at end
	while (step < cycles)
	{
		if (verbose == 1) printf("A:%02X B:%02X C:%02X D:%02X I:%02X F:%02X", c->reg[0], c->reg[1], c->reg[2], c->reg[3], c->ip, c->flags);

		temp = c->mem[c->ip]; // page flipping c->mem[c->page][c->ip]
		switch(c->mem[c->ip])
		{
			case 0x00:															// 0x00
				if (verbose == 1) printf("\t%02X\tNOP", c->mem[c->ip]);
				c->ip++;
				break;
			case 0x01:
				if (verbose == 1) printf("\t%02X\tRET", c->mem[c->ip]);
				c->ip = c->ipstack[0];
				c->ipstack[0] = c->ipstack[1];
				c->ipstack[1] = c->ipstack[2];
				c->ipstack[2] = c->ipstack[3];
				c->ipstack[3] = 0;
				if (verbose == 1) printf("\t\tI: ");
				for(tint=0; tint<4; tint++)
				{
					if (verbose == 1) printf("%02X ", c->ipstack[tint]);
				}
				break;
			case 0x02:
				if (verbose == 1) printf("\t%02X\tPUSHA", c->mem[c->ip]);
				c->stack[15] = c->stack[11];
				c->stack[14] = c->stack[10];
				c->stack[13] = c->stack[9];
				c->stack[12] = c->stack[8];
				c->stack[11] = c->stack[7];
				c->stack[10] = c->stack[6];
				c->stack[9] = c->stack[5];
				c->stack[8] = c->stack[4];
				c->stack[7] = c->stack[3];
				c->stack[6] = c->stack[2];
				c->stack[5] = c->stack[1];
				c->stack[4] = c->stack[0];
				c->stack[3] = c->reg[0];
				c->stack[2] = c->reg[1];
				c->stack[1] = c->reg[2];
				c->stack[0] = c->reg[3];
				c->ip++;
				if (verbose == 1) printf("\t\tS: ");
				for(tint=0; tint<16; tint++)
				{
					if (verbose == 1) printf("%02X ",c->stack[tint]);
				}
				break;
			case 0x03:
				if (verbose == 1) printf("\t%02X\tPOPA", c->mem[c->ip]);
				c->reg[3] = c->stack[0];
				c->reg[2] = c->stack[1];
				c->reg[1] = c->stack[2];
				c->reg[0] = c->stack[3];
				c->stack[0] = c->stack[4];
				c->stack[1] = c->stack[5];
				c->stack[2] = c->stack[6];
				c->stack[3] = c->stack[7];
				c->stack[4] = c->stack[8];
				c->stack[5] = c->stack[9];
				c->stack[6] = c->stack[10];
				c->stack[7] = c->stack[11];
				c->stack[8] = c->stack[12];
				c->stack[9] = c->stack[13];
				c->stack[10] = c->stack[14];
				c->stack[11] = c->stack[15];
				c->stack[12] = 0;
				c->stack[13] = 0;
				c->stack[14] = 0;
				c->stack[15] = 0;
				c->ip++;
				if (verbose == 1) printf("\t\tS: ");
				for(tint=0; tint<16; tint++)
				{
					if (verbose == 1) printf("%02X ",c->stack[tint]);
				}
				break;
			case 0x04:
				if (verbose == 1) printf("\t%02X\tNOT A", c->mem[c->ip]);
				c->reg[0] = 255 - c->reg[0];
				c->ip++;
				break;
			case 0x05:
				if (verbose == 1) printf("\t%02X\tNOT B", c->mem[c->ip]);
				c->reg[1] = 255 - c->reg[1];
				c->ip++;
				break;
			case 0x06:
				if (verbose == 1) printf("\t%02X\tNOT C", c->mem[c->ip]);
				c->reg[2] = 255 - c->reg[2];
				c->ip++;
				break;
			case 0x07:
				if (verbose == 1) printf("\t%02X\tNOT D", c->mem[c->ip]);
				c->reg[3] = 255 - c->reg[3];
				c->ip++;
				break;
			case 0x08:
				if (verbose == 1) printf("\t%02X\tINC A", c->mem[c->ip]);
				c->reg[0]++;
				c->ip++;
				break;
			case 0x09:
				if (verbose == 1) printf("\t%02X\tINC B", c->mem[c->ip]);
				c->reg[1]++;
				c->ip++;
				break;
			case 0x0A:
				if (verbose == 1) printf("\t%02X\tINC C", c->mem[c->ip]);
				c->reg[2]++;
				c->ip++;
				break;
			case 0x0B:
				if (verbose == 1) printf("\t%02X\tINC D", c->mem[c->ip]);
				c->reg[3]++;
				c->ip++;
				break;
			case 0x0C:
				if (verbose == 1) printf("\t%02X\tDEC A", c->mem[c->ip]);
				c->reg[0]--;
				c->ip++;
				break;
			case 0x0D:
				if (verbose == 1) printf("\t%02X\tDEC B", c->mem[c->ip]);
				c->reg[1]--;
				c->ip++;
				break;
			case 0x0E:
				if (verbose == 1) printf("\t%02X\tDEC C", c->mem[c->ip]);
				c->reg[2]--;
				c->ip++;
				break;
			case 0x0F:
				if (verbose == 1) printf("\t%02X\tDEC D", c->mem[c->ip]);
				c->reg[3]--;
				c->ip++;
				break;
			case 0x10:															// 0x10
				if (verbose == 1) printf("\t%02X\tADD A, A", c->mem[c->ip]);
				c->reg[0] = c->reg[0] + c->reg[0];
				c->ip++;
				break;
			case 0x11:
				if (verbose == 1) printf("\t%02X\tADD A, B", c->mem[c->ip]);
				c->reg[0] = c->reg[0] + c->reg[1];
				c->ip++;
				break;
			case 0x12:
				if (verbose == 1) printf("\t%02X\tADD A, C", c->mem[c->ip]);
				c->reg[0] = c->reg[0] + c->reg[2];
				c->ip++;
				break;
			case 0x13:
				if (verbose == 1) printf("\t%02X\tADD A, D", c->mem[c->ip]);
				c->reg[0] = c->reg[0] + c->reg[3];
				c->ip++;
				break;
			case 0x14:
				if (verbose == 1) printf("\t%02X\tADD B, A", c->mem[c->ip]);
				c->reg[1] = c->reg[1] + c->reg[0];
				c->ip++;
				break;
			case 0x15:
				if (verbose == 1) printf("\t%02X\tADD B, B", c->mem[c->ip]);
				c->reg[1] = c->reg[1] + c->reg[1];
				c->ip++;
				break;
			case 0x16:
				if (verbose == 1) printf("\t%02X\tADD B, C", c->mem[c->ip]);
				c->reg[1] = c->reg[1] + c->reg[2];
				c->ip++;
				break;
			case 0x17:
				if (verbose == 1) printf("\t%02X\tADD B, D", c->mem[c->ip]);
				c->reg[1] = c->reg[1] + c->reg[3];
				c->ip++;
				break;
			case 0x18:
				if (verbose == 1) printf("\t%02X\tADD C, A", c->mem[c->ip]);
				c->reg[2] = c->reg[2] + c->reg[0];
				c->ip++;
				break;
			case 0x19:
				if (verbose == 1) printf("\t%02X\tADD C, B", c->mem[c->ip]);
				c->reg[2] = c->reg[2] + c->reg[1];
				c->ip++;
				break;
			case 0x1A:
				if (verbose == 1) printf("\t%02X\tADD C, C", c->mem[c->ip]);
				c->reg[2] = c->reg[2] + c->reg[2];
				c->ip++;
				break;
			case 0x1B:
				if (verbose == 1) printf("\t%02X\tADD C, D", c->mem[c->ip]);
				c->reg[2] = c->reg[2] + c->reg[3];
				c->ip++;
				break;
			case 0x1C:
				if (verbose == 1) printf("\t%02X\tADD D, A", c->mem[c->ip]);
				c->reg[3] = c->reg[3] + c->reg[0];
				c->ip++;
				break;
			case 0x1D:
				if (verbose == 1) printf("\t%02X\tADD D, B", c->mem[c->ip]);
				c->reg[3] = c->reg[3] + c->reg[1];
				c->ip++;
				break;
			case 0x1E:
				if (verbose == 1) printf("\t%02X\tADD D, C", c->mem[c->ip]);
				c->reg[3] = c->reg[3] + c->reg[2];
				c->ip++;
				break;
			case 0x1F:
				if (verbose == 1) printf("\t%02X\tADD D, D", c->mem[c->ip]);
				c->reg[3] = c->reg[3] + c->reg[3];
				c->ip++;
				break;
			case 0x20:															// 0x20
				if (verbose == 1) printf("\t%02X\tSUB A, A", c->mem[c->ip]);
				c->reg[0] = c->reg[0] - c->reg[0];
				c->ip++;
				break;
			case 0x21:
				if (verbose == 1) printf("\t%02X\tSUB A, B", c->mem[c->ip]);
				c->reg[0] = c->reg[0] - c->reg[1];
				c->ip++;
				break;
			case 0x22:
				if (verbose == 1) printf("\t%02X\tSUB A, C", c->mem[c->ip]);
				c->reg[0] = c->reg[0] - c->reg[2];
				c->ip++;
				break;
			case 0x23:
				if (verbose == 1) printf("\t%02X\tSUB A, D", c->mem[c->ip]);
				c->reg[0] = c->reg[0] - c->reg[3];
				c->ip++;
				break;
			case 0x24:
				if (verbose == 1) printf("\t%02X\tSUB B, A", c->mem[c->ip]);
				c->reg[1] = c->reg[1] - c->reg[0];
				c->ip++;
				break;
			case 0x25:
				if (verbose == 1) printf("\t%02X\tSUB B, B", c->mem[c->ip]);
				c->reg[1] = c->reg[1] - c->reg[1];
				c->ip++;
				break;
			case 0x26:
				if (verbose == 1) printf("\t%02X\tSUB B, C", c->mem[c->ip]);
				c->reg[1] = c->reg[1] - c->reg[2];
				c->ip++;
				break;
			case 0x27:
				if (verbose == 1) printf("\t%02X\tSUB B, D", c->mem[c->ip]);
				c->reg[1] = c->reg[1] - c->reg[3];
				c->ip++;
				break;
			case 0x28:
				if (verbose == 1) printf("\t%02X\tSUB C, A", c->mem[c->ip]);
				c->reg[2] = c->reg[2] - c->reg[0];
				c->ip++;
				break;
			case 0x29:
				if (verbose == 1) printf("\t%02X\tSUB C, B", c->mem[c->ip]);
				c->reg[2] = c->reg[2] - c->reg[1];
				c->ip++;
				break;
			case 0x2A:
				if (verbose == 1) printf("\t%02X\tSUB C, C", c->mem[c->ip]);
				c->reg[2] = c->reg[2] - c->reg[2];
				c->ip++;
				break;
			case 0x2B:
				if (verbose == 1) printf("\t%02X\tSUB C, D", c->mem[c->ip]);
				c->reg[2] = c->reg[2] - c->reg[3];
				c->ip++;
				break;
			case 0x2C:
				if (verbose == 1) printf("\t%02X\tSUB D, A", c->mem[c->ip]);
				c->reg[3] = c->reg[3] - c->reg[0];
				c->ip++;
				break;
			case 0x2D:
				if (verbose == 1) printf("\t%02X\tSUB D, B", c->mem[c->ip]);
				c->reg[3] = c->reg[3] - c->reg[1];
				c->ip++;
				break;
			case 0x2E:
				if (verbose == 1) printf("\t%02X\tSUB D, C", c->mem[c->ip]);
				c->reg[3] = c->reg[3] - c->reg[2];
				c->ip++;
				break;
			case 0x2F:
				if (verbose == 1) printf("\t%02X\tSUB D, D", c->mem[c->ip]);
				c->reg[3] = c->reg[3] - c->reg[3];
				c->ip++;
				break;
			case 0x30:															// 0x30
				if (verbose == 1) printf("\t%02X\tMOV A, A", c->mem[c->ip]);
				c->reg[0] = c->reg[0];
				c->ip++;
				break;
			case 0x31:
				if (verbose == 1) printf("\t%02X\tMOV A, B", c->mem[c->ip]);
				c->reg[0] = c->reg[1];
				c->ip++;
				break;
			case 0x32:
				if (verbose == 1) printf("\t%02X\tMOV A, C", c->mem[c->ip]);
				c->reg[0] = c->reg[2];
				c->ip++;
				break;
			case 0x33:
				if (verbose == 1) printf("\t%02X\tMOV A, D", c->mem[c->ip]);
				c->reg[0] = c->reg[3];
				c->ip++;
				break;
			case 0x34:
				if (verbose == 1) printf("\t%02X\tMOV B, A", c->mem[c->ip]);
				c->reg[1] = c->reg[0];
				c->ip++;
				break;
			case 0x35:
				if (verbose == 1) printf("\t%02X\tMOV B, B", c->mem[c->ip]);
				c->reg[1] = c->reg[1];
				c->ip++;
				break;
			case 0x36:
				if (verbose == 1) printf("\t%02X\tMOV B, C", c->mem[c->ip]);
				c->reg[1] = c->reg[2];
				c->ip++;
				break;
			case 0x37:
				if (verbose == 1) printf("\t%02X\tMOV B, D", c->mem[c->ip]);
				c->reg[1] = c->reg[3];
				c->ip++;
				break;
			case 0x38:
				if (verbose == 1) printf("\t%02X\tMOV C, A", c->mem[c->ip]);
				c->reg[2] = c->reg[0];
				c->ip++;
				break;
			case 0x39:
				if (verbose == 1) printf("\t%02X\tMOV C, B", c->mem[c->ip]);
				c->reg[2] = c->reg[1];
				c->ip++;
				break;
			case 0x3A:
				if (verbose == 1) printf("\t%02X\tMOV C, C", c->mem[c->ip]);
				c->reg[2] = c->reg[2];
				c->ip++;
				break;
			case 0x3B:
				if (verbose == 1) printf("\t%02X\tMOV C, D", c->mem[c->ip]);
				c->reg[2] = c->reg[3];
				c->ip++;
				break;
			case 0x3C:
				if (verbose == 1) printf("\t%02X\tMOV D, A", c->mem[c->ip]);
				c->reg[3] = c->reg[0];
				c->ip++;
				break;
			case 0x3D:
				if (verbose == 1) printf("\t%02X\tMOV D, B", c->mem[c->ip]);
				c->reg[3] = c->reg[1];
				c->ip++;
				break;
			case 0x3E:
				if (verbose == 1) printf("\t%02X\tMOV D, C", c->mem[c->ip]);
				c->reg[3] = c->reg[2];
				c->ip++;
				break;
			case 0x3F:
				if (verbose == 1) printf("\t%02X\tMOV D, D", c->mem[c->ip]);
				c->reg[3] = c->reg[3];
				c->ip++;
				break;
			case 0x40:															// 0x40
				if (verbose == 1) printf("\t%02X\tCMP A, A", c->mem[c->ip]);
				c->flags = c->flags | 0x01; // Zero to 1
				c->ip++;
				break;
			case 0x41:
				if (verbose == 1) printf("\t%02X\tCMP A, B", c->mem[c->ip]);
				if (c->reg[0] == c->reg[1])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[0] < c->reg[1])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[0] > c->reg[1])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x42:
				if (verbose == 1) printf("\t%02X\tCMP A, C", c->mem[c->ip]);
				if (c->reg[0] == c->reg[2])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[0] < c->reg[2])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[0] > c->reg[2])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x43:
				if (verbose == 1) printf("\t%02X\tCMP A, D", c->mem[c->ip]);
				if (c->reg[0] == c->reg[3])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[0] < c->reg[3])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[0] > c->reg[3])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x44:
				if (verbose == 1) printf("\t%02X\tCMP B, A", c->mem[c->ip]);
				if (c->reg[1] == c->reg[0])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[1] < c->reg[0])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[1] > c->reg[0])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x45:
				if (verbose == 1) printf("\t%02X\tCMP B, B", c->mem[c->ip]);
				c->flags = c->flags | 0x01; // Zero to 1
				c->ip++;
				break;
			case 0x46:
				if (verbose == 1) printf("\t%02X\tCMP B, C", c->mem[c->ip]);
				if (c->reg[1] == c->reg[2])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[1] < c->reg[2])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[1] > c->reg[2])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x47:
				if (verbose == 1) printf("\t%02X\tCMP B, D", c->mem[c->ip]);
				if (c->reg[1] == c->reg[3])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[1] < c->reg[3])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[1] > c->reg[3])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x48:
				if (verbose == 1) printf("\t%02X\tCMP C, A", c->mem[c->ip]);
				if (c->reg[2] == c->reg[0])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[2] < c->reg[0])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[2] > c->reg[0])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x49:
				if (verbose == 1) printf("\t%02X\tCMP C, B", c->mem[c->ip]);
				if (c->reg[2] == c->reg[1])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[2] < c->reg[1])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[2] > c->reg[1])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x4A:
				if (verbose == 1) printf("\t%02X\tCMP C, C", c->mem[c->ip]);
				c->flags = c->flags | 0x01; // Zero to 1
				c->ip++;
				break;
			case 0x4B:
				if (verbose == 1) printf("\t%02X\tCMP C, D", c->mem[c->ip]);
				if (c->reg[2] == c->reg[3])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[2] < c->reg[3])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[2] > c->reg[3])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x4C:
				if (verbose == 1) printf("\t%02X\tCMP D, A", c->mem[c->ip]);
				if (c->reg[3] == c->reg[0])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[3] < c->reg[0])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[3] > c->reg[0])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x4D:
				if (verbose == 1) printf("\t%02X\tCMP D, B", c->mem[c->ip]);
				if (c->reg[3] == c->reg[1])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[3] < c->reg[1])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[3] > c->reg[1])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x4E:
				if (verbose == 1) printf("\t%02X\tCMP D, C", c->mem[c->ip]);
				if (c->reg[3] == c->reg[2])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[3] < c->reg[2])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[3] > c->reg[2])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip++;
				break;
			case 0x4F:
				if (verbose == 1) printf("\t%02X\tCMP D, D", c->mem[c->ip]);
				c->flags = c->flags | 0x01; // Zero to 1
				c->ip++;
				break;
			case 0x50:															// 0x50
				if (verbose == 1) printf("\t%02X\tAND A, A", c->mem[c->ip]);
				c->reg[0] = c->reg[0] & c->reg[0];
				c->ip++;
				break;
			case 0x51:
				if (verbose == 1) printf("\t%02X\tAND A, B", c->mem[c->ip]);
				c->reg[0] = c->reg[0] & c->reg[1];
				c->ip++;
				break;
			case 0x52:
				if (verbose == 1) printf("\t%02X\tAND A, C", c->mem[c->ip]);
				c->reg[0] = c->reg[0] & c->reg[2];
				c->ip++;
				break;
			case 0x53:
				if (verbose == 1) printf("\t%02X\tAND A, D", c->mem[c->ip]);
				c->reg[0] = c->reg[0] & c->reg[3];
				c->ip++;
				break;
			case 0x54:
				if (verbose == 1) printf("\t%02X\tAND B, A", c->mem[c->ip]);
				c->reg[1] = c->reg[1] & c->reg[0];
				c->ip++;
				break;
			case 0x55:
				if (verbose == 1) printf("\t%02X\tAND B, B", c->mem[c->ip]);
				c->reg[1] = c->reg[1] & c->reg[1];
				c->ip++;
				break;
			case 0x56:
				if (verbose == 1) printf("\t%02X\tAND B, C", c->mem[c->ip]);
				c->reg[1] = c->reg[1] & c->reg[2];
				c->ip++;
				break;
			case 0x57:
				if (verbose == 1) printf("\t%02X\tAND B, D", c->mem[c->ip]);
				c->reg[1] = c->reg[1] & c->reg[3];
				c->ip++;
				break;
			case 0x58:
				if (verbose == 1) printf("\t%02X\tAND C, A", c->mem[c->ip]);
				c->reg[2] = c->reg[2] & c->reg[0];
				c->ip++;
				break;
			case 0x59:
				if (verbose == 1) printf("\t%02X\tAND C, B", c->mem[c->ip]);
				c->reg[2] = c->reg[2] & c->reg[1];
				c->ip++;
				break;
			case 0x5A:
				if (verbose == 1) printf("\t%02X\tAND C, C", c->mem[c->ip]);
				c->reg[2] = c->reg[2] & c->reg[2];
				c->ip++;
				break;
			case 0x5B:
				if (verbose == 1) printf("\t%02X\tAND C, D", c->mem[c->ip]);
				c->reg[2] = c->reg[2] & c->reg[3];
				c->ip++;
				break;
			case 0x5C:
				if (verbose == 1) printf("\t%02X\tAND D, A", c->mem[c->ip]);
				c->reg[3] = c->reg[3] & c->reg[0];
				c->ip++;
				break;
			case 0x5D:
				if (verbose == 1) printf("\t%02X\tAND D, B", c->mem[c->ip]);
				c->reg[3] = c->reg[3] & c->reg[1];
				c->ip++;
				break;
			case 0x5E:
				if (verbose == 1) printf("\t%02X\tAND D, C", c->mem[c->ip]);
				c->reg[3] = c->reg[3] & c->reg[2];
				c->ip++;
				break;
			case 0x5F:
				if (verbose == 1) printf("\t%02X\tAND D, D", c->mem[c->ip]);
				c->reg[3] = c->reg[3] & c->reg[3];
				c->ip++;
				break;
			case 0x60:															// 0x60
				if (verbose == 1) printf("\t%02X\tOR  A, A", c->mem[c->ip]);
				c->reg[0] = c->reg[0] | c->reg[0];
				c->ip++;
				break;
			case 0x61:
				if (verbose == 1) printf("\t%02X\tOR  A, B", c->mem[c->ip]);
				c->reg[0] = c->reg[0] | c->reg[1];
				c->ip++;
				break;
			case 0x62:
				if (verbose == 1) printf("\t%02X\tOR  A, C", c->mem[c->ip]);
				c->reg[0] = c->reg[0] | c->reg[2];
				c->ip++;
				break;
			case 0x63:
				if (verbose == 1) printf("\t%02X\tOR  A, D", c->mem[c->ip]);
				c->reg[0] = c->reg[0] | c->reg[3];
				c->ip++;
				break;
			case 0x64:
				if (verbose == 1) printf("\t%02X\tOR  B, A", c->mem[c->ip]);
				c->reg[1] = c->reg[1] | c->reg[0];
				c->ip++;
				break;
			case 0x65:
				if (verbose == 1) printf("\t%02X\tOR  B, B", c->mem[c->ip]);
				c->reg[1] = c->reg[1] | c->reg[1];
				c->ip++;
				break;
			case 0x66:
				if (verbose == 1) printf("\t%02X\tOR  B, C", c->mem[c->ip]);
				c->reg[1] = c->reg[1] | c->reg[2];
				c->ip++;
				break;
			case 0x67:
				if (verbose == 1) printf("\t%02X\tOR  B, D", c->mem[c->ip]);
				c->reg[1] = c->reg[1] | c->reg[3];
				c->ip++;
				break;
			case 0x68:
				if (verbose == 1) printf("\t%02X\tOR  C, A", c->mem[c->ip]);
				c->reg[2] = c->reg[2] | c->reg[0];
				c->ip++;
				break;
			case 0x69:
				if (verbose == 1) printf("\t%02X\tOR  C, B", c->mem[c->ip]);
				c->reg[2] = c->reg[2] | c->reg[1];
				c->ip++;
				break;
			case 0x6A:
				if (verbose == 1) printf("\t%02X\tOR  C, C", c->mem[c->ip]);
				c->reg[2] = c->reg[2] | c->reg[2];
				c->ip++;
				break;
			case 0x6B:
				if (verbose == 1) printf("\t%02X\tOR  C, D", c->mem[c->ip]);
				c->reg[2] = c->reg[2] | c->reg[3];
				c->ip++;
				break;
			case 0x6C:
				if (verbose == 1) printf("\t%02X\tOR  D, A", c->mem[c->ip]);
				c->reg[3] = c->reg[3] | c->reg[0];
				c->ip++;
				break;
			case 0x6D:
				if (verbose == 1) printf("\t%02X\tOR  D, B", c->mem[c->ip]);
				c->reg[3] = c->reg[3] | c->reg[1];
				c->ip++;
				break;
			case 0x6E:
				if (verbose == 1) printf("\t%02X\tOR  D, C", c->mem[c->ip]);
				c->reg[3] = c->reg[3] | c->reg[2];
				c->ip++;
				break;
			case 0x6F:
				if (verbose == 1) printf("\t%02X\tOR  D, D", c->mem[c->ip]);
				c->reg[3] = c->reg[3] | c->reg[3];
				c->ip++;
				break;
			case 0x70:															// 0x70
				if (verbose == 1) printf("\t%02X\tXOR A, A", c->mem[c->ip]);
				c->reg[0] = c->reg[0] ^ c->reg[0];
				c->ip++;
				break;
			case 0x71:
				if (verbose == 1) printf("\t%02X\tXOR A, B", c->mem[c->ip]);
				c->reg[0] = c->reg[0] ^ c->reg[1];
				c->ip++;
				break;
			case 0x72:
				if (verbose == 1) printf("\t%02X\tXOR A, C", c->mem[c->ip]);
				c->reg[0] = c->reg[0] ^ c->reg[2];
				c->ip++;
				break;
			case 0x73:
				if (verbose == 1) printf("\t%02X\tXOR A, D", c->mem[c->ip]);
				c->reg[0] = c->reg[0] ^ c->reg[3];
				c->ip++;
				break;
			case 0x74:
				if (verbose == 1) printf("\t%02X\tXOR B, A", c->mem[c->ip]);
				c->reg[1] = c->reg[1] ^ c->reg[0];
				c->ip++;
				break;
			case 0x75:
				if (verbose == 1) printf("\t%02X\tXOR B, B", c->mem[c->ip]);
				c->reg[1] = c->reg[1] ^ c->reg[1];
				c->ip++;
				break;
			case 0x76:
				if (verbose == 1) printf("\t%02X\tXOR B, C", c->mem[c->ip]);
				c->reg[1] = c->reg[1] ^ c->reg[2];
				c->ip++;
				break;
			case 0x77:
				if (verbose == 1) printf("\t%02X\tXOR B, D", c->mem[c->ip]);
				c->reg[1] = c->reg[1] ^ c->reg[3];
				c->ip++;
				break;
			case 0x78:
				if (verbose == 1) printf("\t%02X\tXOR C, A", c->mem[c->ip]);
				c->reg[2] = c->reg[2] ^ c->reg[0];
				c->ip++;
				break;
			case 0x79:
				if (verbose == 1) printf("\t%02X\tXOR C, B", c->mem[c->ip]);
				c->reg[2] = c->reg[2] ^ c->reg[1];
				c->ip++;
				break;
			case 0x7A:
				if (verbose == 1) printf("\t%02X\tXOR C, C", c->mem[c->ip]);
				c->reg[2] = c->reg[2] ^ c->reg[2];
				c->ip++;
				break;
			case 0x7B:
				if (verbose == 1) printf("\t%02X\tXOR C, D", c->mem[c->ip]);
				c->reg[2] = c->reg[2] ^ c->reg[3];
				c->ip++;
				break;
			case 0x7C:
				if (verbose == 1) printf("\t%02X\tXOR D, A", c->mem[c->ip]);
				c->reg[3] = c->reg[3] ^ c->reg[0];
				c->ip++;
				break;
			case 0x7D:
				if (verbose == 1) printf("\t%02X\tXOR D, B", c->mem[c->ip]);
				c->reg[3] = c->reg[3] ^ c->reg[1];
				c->ip++;
				break;
			case 0x7E:
				if (verbose == 1) printf("\t%02X\tXOR D, C", c->mem[c->ip]);
				c->reg[3] = c->reg[3] ^ c->reg[2];
				c->ip++;
				break;
			case 0x7F:
				if (verbose == 1) printf("\t%02X\tXOR D, D", c->mem[c->ip]);
				c->reg[3] = c->reg[3] ^ c->reg[3];
				c->ip++;
				break;
			case 0x80:															// 0x80
				if (verbose == 1) printf("\t%02X%02X\tJL  [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);

				if ((c->flags & 0x02) == 0x00) // Carry == 0
					c->ip = c->mem[c->ip+1];
				else
					c->ip+=2;
				break;
			case 0x81:
				if (verbose == 1) printf("\t%02X%02X\tJLE [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				if ((c->flags & 0x02) == 0x00 || (c->flags & 0x01) == 0x01) // Carry = 0 || Zero = 1
					c->ip = c->mem[c->ip+1];
				else
					c->ip+=2;
				break;
			case 0x82:
				if (verbose == 1) printf("\t%02X%02X\tJE  [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				if ((c->flags & 0x01) == 0x01) // Zero = 1
					c->ip = c->mem[c->ip+1];
				else
					c->ip+=2;
				break;
			case 0x83:
				if (verbose == 1) printf("\t%02X%02X\tJGE [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				if ((c->flags & 0x02) == 0x02 || (c->flags & 0x01) == 0x01) // Carry = 1 || Zero = 1
					c->ip = c->mem[c->ip+1];
				else
					c->ip+=2;
				break;
			case 0x84:
				if (verbose == 1) printf("\t%02X%02X\tJG [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				if ((c->flags & 0x02) == 0x02) // Carry = 1
					c->ip = c->mem[c->ip+1];
				else
					c->ip+=2;
				break;
			case 0x85:
				if (verbose == 1) printf("\t%02X%02X\tJMP [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->ip = c->mem[c->ip+1];
				break;
			case 0x86:
				if (verbose == 1) printf("\t%02X%02X\tJNE [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				if ((c->flags & 0x01) == 0x00) // Zero = 0
					c->ip = c->mem[c->ip+1];
				else
					c->ip+=2;
				break;
			case 0x87:
				if (verbose == 1) printf("\t%02X%02X\tCALL [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->ipstack[3] = c->ipstack[2];
				c->ipstack[2] = c->ipstack[1];
				c->ipstack[1] = c->ipstack[0];
				c->ipstack[0] = c->ip+2;
				c->ip = c->mem[c->ip+1];
				if (verbose == 1) printf("\tI: ");
				for(tint=0; tint<4; tint++)
				{
					if (verbose == 1) printf("%02X ",c->ipstack[tint]);
				}
				break;
			case 0x88:
				if (verbose == 1) printf("\t%02X%02X\tMOV [0x%02X], A", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->mem[c->mem[c->ip+1]] = c->reg[0];
				c->ip+=2;
				break;
			case 0x89:
				if (verbose == 1) printf("\t%02X%02X\tMOV [0x%02X], B", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->mem[c->mem[c->ip+1]] = c->reg[1];
				c->ip+=2;
				break;
			case 0x8A:
				if (verbose == 1) printf("\t%02X%02X\tMOV [0x%02X], C", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->mem[c->mem[c->ip+1]] = c->reg[2];
				c->ip+=2;
				break;
			case 0x8B:
				if (verbose == 1) printf("\t%02X%02X\tMOV [0x%02X], D", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->mem[c->mem[c->ip+1]] = c->reg[3];
				c->ip+=2;
				break;
			case 0x8C:
				if (verbose == 1) printf("\t%02X%02X\tMOV A, [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[0] = c->mem[c->mem[c->ip+1]];
				c->ip+=2;
				break;
			case 0x8D:
				if (verbose == 1) printf("\t%02X%02X\tMOV B, [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[1] = c->mem[c->mem[c->ip+1]];
				c->ip+=2;
				break;
			case 0x8E:
				if (verbose == 1) printf("\t%02X%02X\tMOV C, [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[2] = c->mem[c->mem[c->ip+1]];
				c->ip+=2;
				break;
			case 0x8F:
				if (verbose == 1) printf("\t%02X%02X\tMOV D, [0x%02X]", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[3] = c->mem[c->mem[c->ip+1]];
				c->ip+=2;
				break;
			case 0x90:															// 0x90
				if (verbose == 1) printf("\t%02X%02X\tADD A, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[0]+=c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x91:
				if (verbose == 1) printf("\t%02X%02X\tADD B, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[1]+=c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x92:
				if (verbose == 1) printf("\t%02X%02X\tADD C, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[2]+=c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x93:
				if (verbose == 1) printf("\t%02X%02X\tADD D, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[3]+=c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x94:
				if (verbose == 1) printf("\t%02X%02X\tSUB A, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[0]-=c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x95:
				if (verbose == 1) printf("\t%02X%02X\tSUB B, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[1]-=c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x96:
				if (verbose == 1) printf("\t%02X%02X\tSUB C, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[2]-=c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x97:
				if (verbose == 1) printf("\t%02X%02X\tSUB D, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[3]-=c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x98:
				if (verbose == 1) printf("\t%02X%02X\tMOV A, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[0] = c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x99:
				if (verbose == 1) printf("\t%02X%02X\tMOV B, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[1] = c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x9A:
				if (verbose == 1) printf("\t%02X%02X\tMOV C, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[2] = c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x9B:
				if (verbose == 1) printf("\t%02X%02X\tMOV D, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				c->reg[3] = c->mem[c->ip+1];
				c->ip+=2;
				break;
			case 0x9C:
				if (verbose == 1) printf("\t%02X%02X\tCMP A, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				if (c->reg[0] == c->mem[c->ip+1])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[0] < c->mem[c->ip+1])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[0] > c->mem[c->ip+1])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip+=2;
				break;
			case 0x9D:
				if (verbose == 1) printf("\t%02X%02X\tCMP B, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				if (c->reg[1] == c->mem[c->ip+1])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[1] < c->mem[c->ip+1])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[1] > c->mem[c->ip+1])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip+=2;
				break;
			case 0x9E:
				if (verbose == 1) printf("\t%02X%02X\tCMP C, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				if (c->reg[2] == c->mem[c->ip+1])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[2] < c->mem[c->ip+1])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[2] > c->mem[c->ip+1])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip+=2;
				break;
			case 0x9F:
				if (verbose == 1) printf("\t%02X%02X\tCMP D, 0x%02X", c->mem[c->ip], c->mem[c->ip+1], c->mem[c->ip+1]);
				if (c->reg[3] == c->mem[c->ip+1])
					c->flags = c->flags | 0x01; // Zero to 1
				else
				{
					c->flags = c->flags & 0xFE; // Zero to 0
					if (c->reg[3] < c->mem[c->ip+1])
						c->flags = c->flags & 0xFD; // Carry to 0
					else if (c->reg[3] > c->mem[c->ip+1])
						c->flags = c->flags | 0x02; // Carry to 1
				}
				c->ip+=2;
				break;
			case 0xA0:															// 0xA0
				if (verbose == 1) printf("\t%02X\tMOV [&A], A", c->mem[c->ip]);
				c->mem[c->reg[0]] = c->reg[0];
				c->ip++;
				break;
			case 0xA1:
				if (verbose == 1) printf("\t%02X\tMOV [&A], B", c->mem[c->ip]);
				c->mem[c->reg[0]] = c->reg[1];
				c->ip++;
				break;
			case 0xA2:
				if (verbose == 1) printf("\t%02X\tMOV [&A], C", c->mem[c->ip]);
				c->mem[c->reg[0]] = c->reg[2];
				c->ip++;
				break;
			case 0xA3:
				if (verbose == 1) printf("\t%02X\tMOV [&A], D", c->mem[c->ip]);
				c->mem[c->reg[0]] = c->reg[3];
				c->ip++;
				break;
			case 0xA4:
				if (verbose == 1) printf("\t%02X\tMOV [&B], A", c->mem[c->ip]);
				c->mem[c->reg[1]] = c->reg[0];
				c->ip++;
				break;
			case 0xA5:
				if (verbose == 1) printf("\t%02X\tMOV [&B], B", c->mem[c->ip]);
				c->mem[c->reg[1]] = c->reg[1];
				c->ip++;
				break;
			case 0xA6:
				if (verbose == 1) printf("\t%02X\tMOV [&B], C", c->mem[c->ip]);
				c->mem[c->reg[1]] = c->reg[2];
				c->ip++;
				break;
			case 0xA7:
				if (verbose == 1) printf("\t%02X\tMOV [&B], D", c->mem[c->ip]);
				c->mem[c->reg[1]] = c->reg[3];
				c->ip++;
				break;
			case 0xA8:
				if (verbose == 1) printf("\t%02X\tMOV [&C], A", c->mem[c->ip]);
				c->mem[c->reg[2]] = c->reg[0];
				c->ip++;
				break;
			case 0xA9:
				if (verbose == 1) printf("\t%02X\tMOV [&C], B", c->mem[c->ip]);
				c->mem[c->reg[2]] = c->reg[1];
				c->ip++;
				break;
			case 0xAA:
				if (verbose == 1) printf("\t%02X\tMOV [&C], C", c->mem[c->ip]);
				c->mem[c->reg[2]] = c->reg[2];
				c->ip++;
				break;
			case 0xAB:
				if (verbose == 1) printf("\t%02X\tMOV [&C], D", c->mem[c->ip]);
				c->mem[c->reg[2]] = c->reg[3];
				c->ip++;
				break;
			case 0xAC:
				if (verbose == 1) printf("\t%02X\tMOV [&D], A", c->mem[c->ip]);
				c->mem[c->reg[3]] = c->reg[0];
				c->ip++;
				break;
			case 0xAD:
				if (verbose == 1) printf("\t%02X\tMOV [&D], B", c->mem[c->ip]);
				c->mem[c->reg[3]] = c->reg[1];
				c->ip++;
				break;
			case 0xAE:
				if (verbose == 1) printf("\t%02X\tMOV [&D], C", c->mem[c->ip]);
				c->mem[c->reg[3]] = c->reg[2];
				c->ip++;
				break;
			case 0xAF:
				if (verbose == 1) printf("\t%02X\tMOV [&D], D", c->mem[c->ip]);
				c->mem[c->reg[3]] = c->reg[3];
				c->ip++;
				break;
			case 0xB0:															// 0xB0
				if (verbose == 1) printf("\t%02X\tMOV A, [&A]", c->mem[c->ip]);
				c->reg[0] = c->mem[c->reg[0]];
				c->ip++;
				break;
			case 0xB1:
				if (verbose == 1) printf("\t%02X\tMOV A, [&B]", c->mem[c->ip]);
				c->reg[0] = c->mem[c->reg[1]];
				c->ip++;
				break;
			case 0xB2:
				if (verbose == 1) printf("\t%02X\tMOV A, [&C]", c->mem[c->ip]);
				c->reg[0] = c->mem[c->reg[2]];
				c->ip++;
				break;
			case 0xB3:
				if (verbose == 1) printf("\t%02X\tMOV A, [&D]", c->mem[c->ip]);
				c->reg[0] = c->mem[c->reg[3]];
				c->ip++;
				break;
			case 0xB4:
				if (verbose == 1) printf("\t%02X\tMOV B, [&A]", c->mem[c->ip]);
				c->reg[1] = c->mem[c->reg[0]];
				c->ip++;
				break;
			case 0xB5:
				if (verbose == 1) printf("\t%02X\tMOV B, [&B]", c->mem[c->ip]);
				c->reg[1] = c->mem[c->reg[1]];
				c->ip++;
				break;
			case 0xB6:
				if (verbose == 1) printf("\t%02X\tMOV B, [&C]", c->mem[c->ip]);
				c->reg[1] = c->mem[c->reg[2]];
				c->ip++;
				break;
			case 0xB7:
				if (verbose == 1) printf("\t%02X\tMOV B, [&D]", c->mem[c->ip]);
				c->reg[1] = c->mem[c->reg[3]];
				c->ip++;
				break;
			case 0xB8:
				if (verbose == 1) printf("\t%02X\tMOV C, [&A]", c->mem[c->ip]);
				c->reg[2] = c->mem[c->reg[0]];
				c->ip++;
				break;
			case 0xB9:
				if (verbose == 1) printf("\t%02X\tMOV C, [&B]", c->mem[c->ip]);
				c->reg[2] = c->mem[c->reg[1]];
				c->ip++;
				break;
			case 0xBA:
				if (verbose == 1) printf("\t%02X\tMOV C, [&C]", c->mem[c->ip]);
				c->reg[2] = c->mem[c->reg[2]];
				c->ip++;
				break;
			case 0xBB:
				if (verbose == 1) printf("\t%02X\tMOV C, [&D]", c->mem[c->ip]);
				c->reg[2] = c->mem[c->reg[3]];
				c->ip++;
				break;
			case 0xBC:
				if (verbose == 1) printf("\t%02X\tMOV D, [&A]", c->mem[c->ip]);
				c->reg[3] = c->mem[c->reg[0]];
				c->ip++;
				break;
			case 0xBD:
				if (verbose == 1) printf("\t%02X\tMOV D, [&B]", c->mem[c->ip]);
				c->reg[3] = c->mem[c->reg[1]];
				c->ip++;
				break;
			case 0xBE:
				if (verbose == 1) printf("\t%02X\tMOV D, [&C]", c->mem[c->ip]);
				c->reg[3] = c->mem[c->reg[2]];
				c->ip++;
				break;
			case 0xBF:
				if (verbose == 1) printf("\t%02X\tMOV D, [&D]", c->mem[c->ip]);
				c->reg[3] = c->mem[c->reg[3]];
				c->ip++;
				break;
			case 0xC0:															// 0xC0
				if (verbose == 1) printf("\t%02X\tIN A, pA", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[0]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[0] = c->ioport[c->reg[0] & 0x0F];
				i808setportflag(c, c->reg[0], 0);
				c->ip++;
				break;
			case 0xC1:
				if (verbose == 1) printf("\t%02X\tIN A, pB", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[1]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[0] = c->ioport[c->reg[1] & 0x0F];
				i808setportflag(c, c->reg[1], 0);
				c->ip++;
				break;
			case 0xC2:
				if (verbose == 1) printf("\t%02X\tIN A, pC", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[2]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[0] = c->ioport[c->reg[2] & 0x0F];
				i808setportflag(c, c->reg[2], 0);
				c->ip++;
				break;
			case 0xC3:
				if (verbose == 1) printf("\t%02X\tIN A, pD", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[3]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[0] = c->ioport[c->reg[3] & 0x0F];
				i808setportflag(c, c->reg[3], 0);
				c->ip++;
				break;
			case 0xC4:
				if (verbose == 1) printf("\t%02X\tIN B, pA", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[0]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[1] = c->ioport[c->reg[0] & 0x0F];
				i808setportflag(c, c->reg[0], 0);
				c->ip++;
				break;
			case 0xC5:
				if (verbose == 1) printf("\t%02X\tIN B, pB", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[1]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[1] = c->ioport[c->reg[1] & 0x0F];
				i808setportflag(c, c->reg[1], 0);
				c->ip++;
				break;
			case 0xC6:
				if (verbose == 1) printf("\t%02X\tIN B, pC", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[2]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[1] = c->ioport[c->reg[2] & 0x0F];
				i808setportflag(c, c->reg[2], 0);
				c->ip++;
				break;
			case 0xC7:
				if (verbose == 1) printf("\t%02X\tIN B, pD", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[3]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[1] = c->ioport[c->reg[3] & 0x0F];
				i808setportflag(c, c->reg[3], 0);
				c->ip++;
				break;
			case 0xC8:
				if (verbose == 1) printf("\t%02X\tIN C, pA", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[0]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[2] = c->ioport[c->reg[0] & 0x0F];
				i808setportflag(c, c->reg[0], 0);
				c->ip++;
				break;
			case 0xC9:
				if (verbose == 1) printf("\t%02X\tIN C, pB", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[1]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[2] = c->ioport[c->reg[1] & 0x0F];
				i808setportflag(c, c->reg[1], 0);
				c->ip++;
				break;
			case 0xCA:
				if (verbose == 1) printf("\t%02X\tIN C, pC", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[2]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[2] = c->ioport[c->reg[2] & 0x0F];
				i808setportflag(c, c->reg[2], 0);
				c->ip++;
				break;
			case 0xCB:
				if (verbose == 1) printf("\t%02X\tIN C, pD", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[3]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1

				c->reg[2] = c->ioport[c->reg[3] & 0x0F];
				i808setportflag(c, c->reg[3], 0);
				c->ip++;
				break;
			case 0xCC:
				if (verbose == 1) printf("\t%02X\tIN D, pA", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[0]) == 1)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[3] = c->ioport[c->reg[0] & 0x0F];
				i808setportflag(c, c->reg[0], 0);
				c->ip++;
				break;
			case 0xCD:
				if (verbose == 1) printf("\t%02X\tIN D, pB", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[1]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[3] = c->ioport[c->reg[1] & 0x0F];
				i808setportflag(c, c->reg[1], 0);
				c->ip++;
				break;
			case 0xCE:
				if (verbose == 1) printf("\t%02X\tIN D, pC", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[2]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[3] = c->ioport[c->reg[2] & 0x0F];
				i808setportflag(c, c->reg[2], 0);
				c->ip++;
				break;
			case 0xCF:
				if (verbose == 1) printf("\t%02X\tIN D, pD", c->mem[c->ip]);
				if (i808checkportflag(c, c->reg[3]) == 0)
					c->flags = c->flags & 0xFE; // Zero to 0
				else
					c->flags = c->flags | 0x01; // Zero to 1
				c->reg[3] = c->ioport[c->reg[3] & 0x0F];
				i808setportflag(c, c->reg[3], 0);
				c->ip++;
				break;
			case 0xD0:															// 0xD0
				if (verbose == 1) printf("\t%02X\tOUT pA, A", c->mem[c->ip]);
				c->ioport[c->reg[0] & 0x0F] = c->reg[0];
				i808setportflag(c, c->reg[0], 1);
				c->ip++;
				break;
			case 0xD1:
				if (verbose == 1) printf("\t%02X\tOUT pA, B", c->mem[c->ip]);
				c->ioport[c->reg[0] & 0x0F] = c->reg[1];
				i808setportflag(c, c->reg[0], 1);
				c->ip++;
				break;
			case 0xD2:
				if (verbose == 1) printf("\t%02X\tOUT pA, C", c->mem[c->ip]);
				c->ioport[c->reg[0] & 0x0F] = c->reg[2];
				i808setportflag(c, c->reg[0], 1);
				c->ip++;
				break;
			case 0xD3:
				if (verbose == 1) printf("\t%02X\tOUT pA, D", c->mem[c->ip]);
				c->ioport[c->reg[0] & 0x0F] = c->reg[3];
				i808setportflag(c, c->reg[0], 1);
				c->ip++;
				break;
			case 0xD4:
				if (verbose == 1) printf("\t%02X\tOUT pB, A", c->mem[c->ip]);
				c->ioport[c->reg[1] & 0x0F] = c->reg[0];
				i808setportflag(c, c->reg[1], 1);
				c->ip++;
				break;
			case 0xD5:
				if (verbose == 1) printf("\t%02X\tOUT pB, B", c->mem[c->ip]);
				c->ioport[c->reg[1] & 0x0F] = c->reg[1];
				i808setportflag(c, c->reg[1], 1);
				c->ip++;
				break;
			case 0xD6:
				if (verbose == 1) printf("\t%02X\tOUT pB, C", c->mem[c->ip]);
				c->ioport[c->reg[1] & 0x0F] = c->reg[2];
				i808setportflag(c, c->reg[1], 1);
				c->ip++;
				break;
			case 0xD7:
				if (verbose == 1) printf("\t%02X\tOUT pB, D", c->mem[c->ip]);
				c->ioport[c->reg[1] & 0x0F] = c->reg[3];
				i808setportflag(c, c->reg[1], 1);
				c->ip++;
				break;
			case 0xD8:
				if (verbose == 1) printf("\t%02X\tOUT pC, A", c->mem[c->ip]);
				c->ioport[c->reg[2] & 0x0F] = c->reg[0];
				i808setportflag(c, c->reg[2], 1);
				c->ip++;
				break;
			case 0xD9:
				if (verbose == 1) printf("\t%02X\tOUT pC, B", c->mem[c->ip]);
				c->ioport[c->reg[2] & 0x0F] = c->reg[1];
				i808setportflag(c, c->reg[2], 1);
				c->ip++;
				break;
			case 0xDA:
				if (verbose == 1) printf("\t%02X\tOUT pC, C", c->mem[c->ip]);
				c->ioport[c->reg[2] & 0x0F] = c->reg[2];
				i808setportflag(c, c->reg[2], 1);
				c->ip++;
				break;
			case 0xDB:
				if (verbose == 1) printf("\t%02X\tOUT pC, D", c->mem[c->ip]);
				c->ioport[c->reg[2] & 0x0F] = c->reg[3];
				i808setportflag(c, c->reg[2], 1);
				c->ip++;
				break;
			case 0xDC:
				if (verbose == 1) printf("\t%02X\tOUT pD, A", c->mem[c->ip]);
				c->ioport[c->reg[3] & 0x0F] = c->reg[0];
				i808setportflag(c, c->reg[3], 1);
				c->ip++;
				break;
			case 0xDD:
				if (verbose == 1) printf("\t%02X\tOUT pD, B", c->mem[c->ip]);
				c->ioport[c->reg[3] & 0x0F] = c->reg[1];
				i808setportflag(c, c->reg[3], 1);
				c->ip++;
				break;
			case 0xDE:
				if (verbose == 1) printf("\t%02X\tOUT pD, C", c->mem[c->ip]);
				c->ioport[c->reg[3] & 0x0F] = c->reg[2];
				i808setportflag(c, c->reg[3], 1);
				c->ip++;
				break;
			case 0xDF:
				if (verbose == 1) printf("\t%02X\tOUT pD, D", c->mem[c->ip]);
				c->ioport[c->reg[3] & 0x0F] = c->reg[3];
				i808setportflag(c, c->reg[3], 1);
				c->ip++;
				break;
			case 0xE0:															// 0xE0
				if (verbose == 1) printf("\t%02X\tPUSH A", c->mem[c->ip]);
				for(tint=15; tint>0; tint--)
					c->stack[tint] = c->stack[tint-1];
				c->stack[0] = c->reg[0];
				c->ip++;
				break;
			case 0xE1:
				if (verbose == 1) printf("\t%02X\tPUSH B", c->mem[c->ip]);
				for(tint=15; tint>0; tint--)
					c->stack[tint] = c->stack[tint-1];
				c->stack[0] = c->reg[1];
				c->ip++;
				break;
			case 0xE2:
				if (verbose == 1) printf("\t%02X\tPUSH C", c->mem[c->ip]);
				for(tint=15; tint>0; tint--)
					c->stack[tint] = c->stack[tint-1];
				c->stack[0] = c->reg[2];
				c->ip++;
				break;
			case 0xE3:
				if (verbose == 1) printf("\t%02X\tPUSH D", c->mem[c->ip]);
				for(tint=15; tint>0; tint--)
					c->stack[tint] = c->stack[tint-1];
				c->stack[0] = c->reg[3];
				c->ip++;
				break;
			case 0xE4:
				if (verbose == 1) printf("\t%02X\tPOP A", c->mem[c->ip]);
				c->reg[0] = c->stack[0];
				for (tint=0; tint<15; tint++)
					c->stack[tint] = c->stack[tint+1];
				c->stack[15] = 0;
				c->ip++;
				break;
			case 0xE5:
				if (verbose == 1) printf("\t%02X\tPOP B", c->mem[c->ip]);
				c->reg[1] = c->stack[0];
				for (tint=0; tint<15; tint++)
					c->stack[tint] = c->stack[tint+1];
				c->stack[15] = 0;
				c->ip++;
				break;
			case 0xE6:
				if (verbose == 1) printf("\t%02X\tPOP C", c->mem[c->ip]);
				c->reg[2] = c->stack[0];
				for (tint=0; tint<15; tint++)
					c->stack[tint] = c->stack[tint+1];
				c->stack[15] = 0;
				c->ip++;
				break;
			case 0xE7:
				if (verbose == 1) printf("\t%02X\tPOP D", c->mem[c->ip]);
				c->reg[3] = c->stack[0];
				for (tint=0; tint<15; tint++)
					c->stack[tint] = c->stack[tint+1];
				c->stack[15] = 0;
				c->ip++;
				break;
			case 0xE8:
				if (verbose == 1) printf("\t%02X\tBSL A", c->mem[c->ip]);
				c->reg[0] = c->reg[0] << 1;
				c->ip++;
				break;
			case 0xE9:
				if (verbose == 1) printf("\t%02X\tBSL B", c->mem[c->ip]);
				c->reg[1] = c->reg[1] << 1;
				c->ip++;
				break;
			case 0xEA:
				if (verbose == 1) printf("\t%02X\tBSL C", c->mem[c->ip]);
				c->reg[2] = c->reg[2] << 1;
				c->ip++;
				break;
			case 0xEB:
				if (verbose == 1) printf("\t%02X\tBSL D", c->mem[c->ip]);
				c->reg[3] = c->reg[3] << 1;
				c->ip++;
				break;
			case 0xEC:
				if (verbose == 1) printf("\t%02X\tBSR A", c->mem[c->ip]);
				c->reg[0] = c->reg[0] >> 1;
				c->ip++;
				break;
			case 0xED:
				if (verbose == 1) printf("\t%02X\tBSR B", c->mem[c->ip]);
				c->reg[1] = c->reg[1] >> 1;
				c->ip++;
				break;
			case 0xEE:
				if (verbose == 1) printf("\t%02X\tBSR C", c->mem[c->ip]);
				c->reg[2] = c->reg[2] >> 1;
				c->ip++;
				break;
			case 0xEF:
				if (verbose == 1) printf("\t%02X\tBSR D", c->mem[c->ip]);
				c->reg[3] = c->reg[3] >> 1;
				c->ip++;
				break;
			case 0xF0:															// 0xF0
				if (verbose == 1) printf("\t%02X\tJSF A", c->mem[c->ip]);
				c->ip += c->reg[0];
				c->ip++;
				break;
			case 0xF1:
				if (verbose == 1) printf("\t%02X\tJSF B", c->mem[c->ip]);
				c->ip += c->reg[1];
				c->ip++;
				break;
			case 0xF2:
				if (verbose == 1) printf("\t%02X\tJSF C", c->mem[c->ip]);
				c->ip += c->reg[2];
				c->ip++;
				break;
			case 0xF3:
				if (verbose == 1) printf("\t%02X\tJSF D", c->mem[c->ip]);
				c->ip += c->reg[3];
				c->ip++;
				break;
			case 0xF4:
				if (verbose == 1) printf("\t%02X\tJSB A", c->mem[c->ip]);
				c->ip -= c->reg[0];
				if (c->reg[0] == 0) // stop from getting stuck in a backwards jump of 0
					c->ip++;
				break;
			case 0xF5:
				if (verbose == 1) printf("\t%02X\tJSB B", c->mem[c->ip]);
				c->ip -= c->reg[1];
				if (c->reg[1] == 0)
					c->ip++;
				break;
			case 0xF6:
				if (verbose == 1) printf("\t%02X\tJSB C", c->mem[c->ip]);
				c->ip -= c->reg[2];
				if (c->reg[2] == 0)
					c->ip++;
				break;
			case 0xF7:
				if (verbose == 1) printf("\t%02X\tJSB D", c->mem[c->ip]);
				c->ip -= c->reg[3];
				if (c->reg[3] == 0)
					c->ip++;
				break;
			case 0xF8:
				if (verbose == 1) printf("\t%02X\tRND A", c->mem[c->ip]);
				c->reg[0] = rand() % 256;
				c->ip++;
				break;
			case 0xF9:
				if (verbose == 1) printf("\t%02X\tRND B", c->mem[c->ip]);
				c->reg[1] = rand() % 256;
				c->ip++;
				break;
			case 0xFA:
				if (verbose == 1) printf("\t%02X\tRND C", c->mem[c->ip]);
				c->reg[2] = rand() % 256;
				c->ip++;
				break;
			case 0xFB:
				if (verbose == 1) printf("\t%02X\tRND D", c->mem[c->ip]);
				c->reg[3] = rand() % 256;
				c->ip++;
				break;
			case 0xFC:
				if (verbose == 1) printf("\t%02X\tUnimplemented opcode", c->mem[c->ip]);
				c->ip++;
				break;
			case 0xFD:
				if (verbose == 1) printf("\t%02X\tUnimplemented opcode", c->mem[c->ip]);
				c->ip++;
				break;
			case 0xFE:
				if (verbose == 1) printf("\t%02X\tUnimplemented opcode", c->mem[c->ip]);
				c->ip++;
				break;
			case 0xFF:
				if (verbose == 1) printf("\t%02X\tUnimplemented opcode", c->mem[c->ip]);
				c->ip++;
				break;
			default:	// We have a case for each situation so we should never hit this
				if (verbose == 1) printf("\t%02X\tUnimplemented opcode", c->mem[c->ip]);
				c->ip++;
				break;
		}
		if (verbose == 1)
		{
			if ((temp >= 0x40 && temp <= 0x4F) || (temp >= 0x9C && temp <= 0x9F))
				printf("\tZ=%d, C=%d", i808checkflag(c, 0), i808checkflag(c, 1));
		}
		step++;
	}
}


/*========================================================*
 * Function: i808show()
 * Purpose : Show a dump of an i808 computer
 * Entry   : eightbitComputer *c = Pointer to a computer
 * Returns : nothing
 *========================================================*/

void i808show(eightbitComputer *c)
{
	int k = 0, h = 0, i = 0;
	printf("\n/----/------------------ Memory Dump ------------------\\");
	printf("\n| 0x | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |");
	printf("\n|----|-------------------------------------------------|");
	printf("\n| 00 | ");
	for (k=0; k<256; k++)
	{
		printf("%02X ", c->mem[k]); // hexadecimal
		// printf("%03d ", c->mem[k]); // decimal
		h++;
		if (h > 15)
		{
			printf("|\n");
			h = 0;
			if (i < 0xEF)
				printf("| %02X | ", i+=16);				
		}
	}
	printf("\\----\\-------------------------------------------------/\n");
	printf("A:%02X\tB:%02X\tC:%02X\tD:%02X\tIP:%02X\tZero:%d Carry:%d\n", c->reg[0], c->reg[1], c->reg[2], c->reg[3], c->ip, i808checkflag(c, 0), i808checkflag(c, 1));
	printf("   Stack: ");
	for (k=0; k<16; k++)
		printf("%02X ", c->stack[k]);
	printf("\nIP Stack: ");
	for (k=0; k<4; k++)
		printf("%02X ", c->ipstack[k]);
	printf("\n      IO: ");
	for (k=0; k<16; k++)
		printf("%02X ", c->ioport[k]);
	printf("\n IO Flag: ");
	for (k=0; k<16; k++)
		printf("%2d ", i808checkportflag(c, k));
	printf("\n");
}


/*========================================================*
 * Function: i808reset()
 * Purpose : Reset an i808 computer
 * Entry   : eightbitComputer *c = Pointer to a computer
 * Returns : nothing
 *========================================================*/

void i808reset(eightbitComputer *c)
{
	unsigned char tchar = 0;

	for (tchar=0; tchar<4; tchar++)
	{
		c->reg[tchar] = 0x00;
		c->ipstack[tchar] = 0x00;
	}
	for (tchar=0; tchar<16; tchar++)
	{
		c->stack[tchar] = 0x00;
		c->ioport[tchar] = 0x00;
	}
	c->ip = 0x00;
	c->flags = 0x00;
	c->ioportflag[0]=0x00;
	c->ioportflag[1]=0x00;
}


/*========================================================*
 * Function: i808readport()
 * Purpose : Reads a value from an i808 computer port
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 unsigned char port = Port to read from (0-15)
 * Returns : unsigned char from port
 *========================================================*/

unsigned char i808readport(eightbitComputer *c, unsigned char port)
{
	unsigned char tchar = port & 0x0F; // remove high nibble, only 0 - 15 are valid

	if (i808checkportflag(c, tchar) == 1)
	{
		i808setportflag(c, tchar, 0);
		return c->ioport[tchar];
	}
	else
	{
		return 0;
	}
}


/*========================================================*
 * Function: i808writeport()
 * Purpose : Writes a value to an i808 computer port
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 unsigned char port = Port to write to (0-15)
 *			 unsigned char val = Value to write
 * Returns : nothing
 *========================================================*/

void i808writeport(eightbitComputer *c, unsigned char port, unsigned char val)
{
	unsigned char tchar = port & 0x0F; // remove high nibble, only 0 - 15 are valid

	c->ioport[tchar] = val;
	i808setportflag(c, tchar, 1);
}


/*========================================================*
 * Function: i808setportflag()
 * Purpose : Sets a port flag
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 port = Port to set
 *			 val = Value to set (0 or 1)
 * Returns : Nothing
 *========================================================*/

void i808setportflag(eightbitComputer *c, unsigned char port, unsigned char val)
{
	unsigned char tchar = port & 0x0F; // remove high nibble, only 0 - 15 are valid
	
	if (tchar < 8) // low byte
	{
		if (val == 0)
		{
			c->ioportflag[1] = c->ioportflag[1] & bit8fieldh[tchar]; // set flag to 0
		}
		else
		{
			c->ioportflag[1] = c->ioportflag[1] | bit8fieldl[tchar]; // set flag to 1
		}
	}
	else // high byte
	{
		tchar = tchar - 8;
		if (val == 0)
		{
			c->ioportflag[0] = c->ioportflag[0] & bit8fieldh[tchar]; // set flag to 0
		}
		else
		{
			c->ioportflag[0] = c->ioportflag[0] | bit8fieldl[tchar]; // set flag to 1
		}
	}	
}


/*========================================================*
 * Function: i808checkportflag()
 * Purpose : Checks a port flag
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 port = Port to check
 * Returns : Flag on port (1 or 0)
 *========================================================*/

unsigned char i808checkportflag(eightbitComputer *c, unsigned char port)
{
	unsigned char tchar = port & 0x0F; // remove high nibble, only 0 - 15 are valid
	
	if (tchar < 8) // low byte
	{
		if ((c->ioportflag[1] & bit8fieldl[tchar]) > 0) // it is 1
			return 1;
		else
			return 0;
	}
	else // high byte
	{
		tchar = tchar - 8;
		if ((c->ioportflag[0] & bit8fieldl[tchar]) > 0) // it is 1
			return 1;
		else
			return 0;		
	}
}


/*========================================================*
 * Function: i808setflag()
 * Purpose : Sets a flag
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 flag = Flag to set
 *			 val = Value to set (0 or 1)
 * Returns : Nothing
 *========================================================*/

void i808setflag(eightbitComputer *c, unsigned char flag, unsigned char val)
{
	if (val == 0)
	{
		c->flags = c->flags & bit8fieldh[flag]; // set flag to 0
	}
	else if (val == 1)
	{
		c->flags = c->flags | bit8fieldl[flag]; // set flag to 1
	}
	else
	{
		printf("i808 Library: Flag set error! No change was made.\n");
	}
}


/*========================================================*
 * Function: i808checkflag()
 * Purpose : Checks a flag
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 flag = Flag to check
 * Returns : Flag on port (1 or 0)
 *========================================================*/

unsigned char i808checkflag(eightbitComputer *c, unsigned char flag)
{
	if ((c->flags & bit8fieldl[flag]) > 0) // it is 1
		return 1;
	else
		return 0;
}


/*========================================================*
 * Function: i808savestate()
 * Purpose : Save the state of an i808 to file
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 char *statefilename = Name of state file
 * Returns : Success or Failure
 *========================================================*/

unsigned char i808savestate(eightbitComputer *c, char *statefilename)
{
	FILE *statefile;

	if ((statefile = fopen(statefilename, "wb")) == NULL)
	{
		printf("\ni808 Error:\nError saving state file %s\n", statefilename);
		return 1;
	}	
	fwrite(c, sizeof(eightbitComputer), 1, statefile);
	fclose(statefile);

	return 0;	 
}


/*========================================================*
 * Function: i808loadstate()
 * Purpose : Load the state of an i808 from file
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 char *statefilename = Name of state file
 * Returns : Success or Failure
 *========================================================*/

unsigned char i808loadstate(eightbitComputer *c, char *statefilename)
{
	FILE *statefile;

	if ((statefile = fopen(statefilename, "rb")) == NULL)
	{
		printf("\ni808 Error:\nError loading state file %s\n", statefilename);
		return 1;
	}
	fread(c, sizeof(eightbitComputer), 1, statefile);
	fclose(statefile);

	return 0;
}


/*========================================================*
 * Function: i808saveprogram()
 * Purpose : Save the program of an i808 to file
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 char *programfilename = Name of program file
 * Returns : Success or Failure
 *========================================================*/

unsigned char i808saveprogram(eightbitComputer *c, char *programfilename)
{
	FILE *programfile;

	if ((programfile = fopen(programfilename, "wb")) == NULL)
	{
		printf("\ni808 Error:\nError saving program file %s\n", programfilename);
		return 1;
	}	
	fwrite(c->mem, sizeof(c->mem), 1, programfile);
	fclose(programfile);

	return 0;	 
}


/*========================================================*
 * Function: i808loadprogram()
 * Purpose : Load a program to an i808 from file
 * Entry   : eightbitComputer *c = Pointer to a computer
 *			 char *programfilename = Name of program file
 * Returns : Success or Failure
 *========================================================*/

unsigned char i808loadprogram(eightbitComputer *c, char *programfilename)
{
	FILE *programfile;

	if ((programfile = fopen(programfilename, "rb")) == NULL)
	{
		printf("\ni808 Error:\nError loading program file %s\n", programfilename);
		return 1;
	}
	fread(c->mem, sizeof(c->mem), 1, programfile);
	fclose(programfile);

	return 0;
}
