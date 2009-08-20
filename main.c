#include <stdio.h>
#include <string.h>
#include "simulator.h"

int step_mode = 0;

// Command line Argument parser
int ParseArgument(int argc, char** argv)
{
	int i;
	for(i=0;i<argc;i++)
	{
		if(strcmp(argv[i], "-h") == 0)
		{
			printf("Usage: %s [options]\n", argv[0]);
			printf("Options:\n");
			printf("\t-h\t Display this help\n");
			printf("\t-s\t Steping mode\n");
			printf("\t-c\t Color mode (UNIX only)\n");
			return 1;
		} else if(strcmp(argv[i], "-s") == 0)
		{
			step_mode = 1;
		} else if(strcmp(argv[i], "-c") == 0)
			COLOR = 1;
	}
	return 0;
}

// 32K Memory Dump
void MemDump(char* filename)
{
	int i;
	FILE *fp;
	fp = fopen(filename, "w");
	RegDump(fp);
	fprintf(fp, "Memory Dump:");
	for(i=0;i<MEM_SIZE;i++)
	{
		if(i % 36 == 0) fprintf(fp, "\nmem[%04x]", i);
		if(i % 3 == 0) fprintf(fp, " ");
		fprintf(fp, "%02x", mem[i] & 0xFF);
	}
	fclose(fp);
}
int main(int argc, char** argv)
{
	int retval = 0;

	if(ParseArgument(argc, argv))
		return 0;

	Init(); // initialize simulator

	// Simulation
	while(retval == 0)
	{
		if(step_mode)
			printf("Executing %02x%02x%02x\n", mem[PC], mem[PC+1], mem[PC+2]);
		retval = Step();
		if(retval == 0 && step_mode)
		{
			Dump(LastTA);
			getchar();
		}
	}
	printf("-----------------------------------------\n");
	printf("Simulation terminated\n");

	// Memory Dump
	MemDump("memdump.txt");
	printf("Memory dumped to file memdump.txt\n");
	CloseAllDevice();
	return 0;
}
