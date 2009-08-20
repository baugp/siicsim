/*
 * SIC CPU Simulator
 *
 * ! sizeof(char) must equals 1 on Host Machine
 * - Big-endian CPU
 * - 32Kbytes Memory, byte addressing
 * - 5 Registers: A, X, L, PC, SW
 * - 26 Instructions
 *
 */

#include<stdio.h>
#include "simulator.h"
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

unsigned char mem[MEM_SIZE] = {0};		// Memory
uint A, X, L, PC;						// Register
StatusWord SW;
FILE *dev[256] = {0};					// Device Pointers
uint LastTA;
int COLOR = 0;

void PrintError(char* func, char* message)
{
	printf("%s(): %s\n", func, message);
}

/*
 * Provide two complement addition of 3-byte integers
 */
uint TwoCompAdd(uint a, uint b)
{
	return (a + b) & 0xFFFFFF;
}

uint TwoComp(uint x)
{
	return ((~x)+1) & 0xFFFFFF;
}

void RegDump(FILE* fp)
{
	// register dump
	fprintf(fp, "Register Dump:\n");
	fprintf(fp, "A = 0x%06x	X = 0x%06x	L = 0x%06x\n",
			A & 0xFFFFFF, X & 0xFFFFFF, L & 0xFFFFFF);
	fprintf(fp, "PC = 0x%06x	SW = 0x%06x\n", 
			PC & 0xFFFFFF, *((uint*)&SW));
}

void Dump(int TA)
{
	int i;
	RegDump(stdout);
	printf("Mem Dump:");
	for(i=max(0, (TA/24-1)*24);i<min(MEM_SIZE, (TA/24+2)*24);i++)
	{
		if(i % 24 == 0) printf("\nmem[%04x]", i);
		if(i % 3 == 0) printf(" ");
		if(COLOR && i >= TA && i < TA+3)
		{
			printf("\033[22;31m%02x\033[0m", mem[i] & 0xFF);
		} else 
			printf("%02x", mem[i] & 0xFF);
	}
	printf("\n");
}

int LoadBootStrapLoader()
{
	FILE* fp;
	int i;

	fp = fopen("dev/00", "rb");
	if(fp == NULL) return 1;	// Error reading dev 00
	
	// read 128 byte to memory
	for(i = 0; i < 128 && !feof(fp); i++)
	{
		fscanf(fp, "%02x", (unsigned int*)(mem+i));
	}
	
	fclose(fp);
	return 0;
}

void StoreInt(uint TA, uint value)
{
	mem[TA] = value>>16;
	mem[TA+1] = (value & 0xFF00) >> 8;
	mem[TA+2] = value & 0xFF; 
}

void Compare(uint a, uint b)
{
	uint temp;
	temp = TwoCompAdd(a, TwoComp(b));		// perform A-B
	if(temp == 0)
		SW.CC = EQUAL;
	else if (temp & 0x800000)				// if result is negative
		SW.CC = LESSER;
	else									// if result is positive
		SW.CC = GREATER;
}

void TestDevice(unsigned char id)
{
	char filename[10];
	if(dev[id] == NULL)		// file not openned, try open it!
	{
		sprintf(filename, "dev/%02x", id);
		dev[id] = fopen(filename, "w+");
	}

	if(dev[id] == NULL)
		SW.CC = EQUAL;		// device not ready
	else
		SW.CC = LESSER;		// device ready
}

void ReadDevice(unsigned char id)
{
	if(dev[id] != NULL && !feof(dev[id]))
	{
		A = fgetc(dev[id]) & 0xFF;
	} else {
		//TODO: handle exception when we can't read from a device
		A = 0xFFFFFF; // -1
	}
}

void WriteDevice(unsigned char id)
{
	if(dev[id] != NULL)
	{
		fputc(A & 0xFF, dev[id]);
		fflush(dev[id]);
	}
}

void CloseAllDevice()
{
	int i;
	for(i=0;i<256;i++)
		if(dev[i])
			fclose(dev[i]);
}

int Execute(unsigned char op, uint TA)
{
	uint operand;

	// fetch operand
	if(TA >= MEM_SIZE)
		return 2;	// Memory access out of range
	operand = (mem[TA]<<16) + (mem[TA+1]<<8) + mem[TA+2];

	switch(op)
	{
		case 0x18:				// ADD
			A = TwoCompAdd(A, operand);	// A = A + operand
			break;
		case 0x40:				// AND
			A = A & operand;
			break;
		case 0x28:				// COMP
			Compare(A, operand);
			break;
		case 0x24:				// DIV
			A = A / operand;
			break;
		case 0x3C:				// J
			PC = TA;
			break;
		case 0x30:				// JEQ
			if(SW.CC == EQUAL)
				PC = TA;
			break;
		case 0x34:				// JGT
			if(SW.CC == GREATER)
				PC = TA;
			break;
		case 0x38:				// JLT
			if(SW.CC == LESSER)
				PC = TA;
			break;
		case 0x48:				// JSUB
			L = PC;
			PC = TA;
			break;
		case 0x00:				// LDA
			A = operand;
			break;
		case 0x50:				// LDCH
			A = mem[TA];
			break;
		case 0x08:				// LDL
			L = operand;
			break;
		case 0x04:				// LDX
			X = operand;
			break;
		case 0x20:				// MUL
			A = (A * operand) & 0xFFFFFF;
			break;
		case 0x44:				// OR
			A = A | operand;
			break;
		case 0xD8:				// RD (read device)
			ReadDevice(mem[TA]);
			break;
		case 0x4C:				// RSUB
			PC = L;
			break;
		case 0x0C:				// STA
			StoreInt(TA, A);
			break;
		case 0x54:				// STCH
			mem[TA] = A & 0xFF;
			break;
		case 0x14:				// STL
			StoreInt(TA, L);
			break;
		case 0xE8:				// STSW
			StoreInt(TA, *((uint*)&SW));
			break;
		case 0x10:				// STX
			StoreInt(TA, X);
			break;
		case 0x1C:				// SUB
			A = TwoCompAdd(A, TwoComp(operand));	// A = A - operand
			break;
		case 0xE0:				// TD (test device)
			TestDevice(mem[TA]);
			break;
		case 0x2C:				// TIX  (increment and compare)
			X = TwoCompAdd(X, 1);
			Compare(X, operand);
			break;
		case 0xDC:				// WD (write device)
			WriteDevice(mem[TA]);
			break;
		default:
			return 1;	// Invalid opcode
	}
	return 0;	// OK
}

int Init()
{
	if(LoadBootStrapLoader())
	{
		PrintError("LoadBootStrapLoader", "Error reading dev00");
		return 1;
	}

	// initialize PC
	PC = 0;
	L = -1;	// for RSUB
	return 0;
}

int Step()
{
	unsigned char op;
	uint TA;
	int retval;

	// fetch
	op = mem[PC];

	// decode (calculate TA)
	TA = ((mem[PC+1] & 0x7F) << 8) + mem[PC+2];
	// index mode?
	if(mem[PC+1] && 0x80)
		TA = TwoCompAdd(TA, X);
	LastTA = TA;

	// point PC to a next instruction
	PC += 3;

	// execute
	if((retval = Execute(op, TA)))
	{
		printf("Exception at PC = 0x%06x (value = %02x%02x%02x)\n", PC-3, mem[PC-3], mem[PC-2], mem[PC-1]);
		if(retval == 1)
			PrintError("\tExecute", "Unknown opcode");
		else if(retval == 2)
			PrintError("\tExecute", "Memory access violation (out of range)");
		else if(retval == 99)
			PrintError("\tExecute", "Not implemented");
		else
			PrintError("\tExecute", "unknown exception");
		return 1;
	}
	if(PC == -1)
	{
		return 1;
	}

	return 0;
}

