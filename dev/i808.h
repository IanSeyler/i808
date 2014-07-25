/*
i808.h - Header file for i808 Virtual Computer
v1.0b - 2007.11.15
Coded by Ian Seyler (iseyler@gmail.com)
*/

/* Structure of an i808 Computer */
struct i808Struct {
	unsigned char mem[256];
	unsigned char reg[4]; // A, B, C, D
	unsigned char ip;
	unsigned char flags; // bits: unused, unused, unused, unused, unused, unused, carry, zero
	unsigned char stack[16];
	unsigned char ipstack[4];
	unsigned char ioport[16];
	unsigned char ioportflag[2]; // 0 high, 1 low
	unsigned char unused[20]; // filler space, total size of i808Struct is 320 bytes
};

/* Typedef for the structure */
typedef struct i808Struct eightbitComputer;

/* Function prototypes */
void i808compute(eightbitComputer *c, unsigned int cycles, unsigned int verbose);
void i808show(eightbitComputer *c);
void i808reset(eightbitComputer *c);
unsigned char i808readport(eightbitComputer *c, unsigned char port);
void i808writeport(eightbitComputer *c, unsigned char port, unsigned char val);
void i808setportflag(eightbitComputer *c, unsigned char port, unsigned char val);
unsigned char i808checkportflag(eightbitComputer *c, unsigned char port);
void i808setflag(eightbitComputer *c, unsigned char flag, unsigned char val);
unsigned char i808checkflag(eightbitComputer *c, unsigned char flag);
unsigned char i808savestate(eightbitComputer *c, char *statefilename);
unsigned char i808loadstate(eightbitComputer *c, char *statefilename);
unsigned char i808saveprogram(eightbitComputer *c, char *programfilename);
unsigned char i808loadprogram(eightbitComputer *c, char *programfilename);
