#ifndef SIMULATOR_H

#define SIMULATOR_H

#define uint unsigned int
#define MEM_SIZE 32767

enum { EQUAL, LESSER, GREATER };	// CC (condition code) enum
typedef struct {
	uint MODE : 1;		// 0=user mode, 1=supervisor mode
	uint IDLE : 1;		// 0=running, 1=idle
	uint ID : 4;		// Process identifier
	uint CC : 2;		// Condition Code
	uint MASK : 4;		// Interrupt Mask
	uint unused : 4;	// unused
	uint ICODE : 8;		// Interruption code
	uint __PADDING : 8;
} StatusWord;

extern unsigned char mem[MEM_SIZE];
extern uint A, X, L, PC;
extern StatusWord SW;
extern uint LastTA;
extern int COLOR;

int Step();
int Init();
void CloseAllDevice();
void RegDump(FILE* fp);
void Dump();

#endif
