// Homework 1: PM/0 Virtual Machine
// Garrett Miller 2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int halt = 0;
static int dll = 0;		// Dynamic Link Level : Amount of activation records

// Process Address Space
static int pas[500];
	
static int bp = 499;	// base pointer for activation records
static int sp = 500;	// stack pointer for top of stack, growing down
static int pc = 10;		// program counter

typedef struct IR IR;

struct IR {
	int op;	// Operation Code
	int l;	// Lexigraphical level
	int m;	// Indicator for operation performed
};

IR ir;


/********************************************/
/*		Find base L levels down				*/
/*											*/
/********************************************/
 
int base(int BP, int L)
{
	int arb = BP;	//arb = activation record base
	while (L > 0)	//find base L levels down
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}

int dlbase(int BP, int L)
{
	int arb = BP;	//arb = activation record base
	while (L > 0)	//find base L levels down
	{
		arb = pas[arb - 1];	// Use dynamic link
		L--;
	}
	return arb;
}

void printValues() {
	// Print Instrution values and Program values
	printf("%-3d  %-3d  ", ir.l, ir.m);
	printf("%-3d  %-3d  %-3d  ", pc, bp, sp);
	
	int L = dll;
	int arb = dlbase(bp, L);
	// Print Stack
	for (int i = 499; i >= sp; i--) {
		if (i == arb) {
			if (i != 499) {
				printf("| ");
			}
			L--;
			arb = dlbase(bp, L);
		}
		printf("%d ", pas[i]);
	}
	printf("\n");
}

int main(int argc, char ** argv) {
	
	// Process input file for instuctions
	FILE * input = fopen(argv[1], "r");
	int i = 0, text = pc;

	fscanf(input, "%d", &i);
	while (!feof(input)) {
		pas[text++] = i;
		fscanf(input, "%d", &i);
	}
	pas[text] = i;
	
	fclose(input);
	
	

#ifdef DEBUG
	for (int i = 0; i < 500; i++) {
		printf("%d ", pas[i]);
		if (i % 2 == 0) {
			printf("\n");
		}
	}
#endif
	printf("%-17s  %-3s  %-3s  %-3s  %s\n", "","PC", "BP", "SP", "stack");
	printf("%-17s  ", "Initial Values: ");	
	printf("%-3d  %-3d  %-3d  \n\n", pc, bp, sp);
	
	/* Main Program */
	while (!halt) {	
		
		// Fetch Instruction
		ir.op = pas[pc];
		ir.l = pas[pc + 1];
		ir.m = pas[pc + 2];
		pc += 3;
			
		// Execute
		switch (ir.op) {
			
			/* Place a literal on top of stack. */
			case(1):	// Lit 0,M	
				sp--;
				pas[sp] = ir.m;
				
				printf("%-9s", "LIT");
				break;
			
			/* Operation to be performed on the data on the top of the stack. */
			case(2):	// OPR 0,M	
				
				switch (ir.m) {
					case(0):	// RET
						sp = bp + 1;
						bp = pas[sp - 2];
						pc = pas[sp - 3];
						
						dll--;
						printf("%-9s", "RET");
						break;
					
					case(1):	// ADD
						pas[sp + 1] = pas[sp + 1] + pas[sp];
						sp++;
							
						printf("%-9s", "ADD");
						break;
					
					case(2):	// SUB
						pas[sp + 1] = pas[sp + 1] - pas[sp];
						sp++;
						
						
						printf("%-9s", "SUB");
						break;
					
					case(3):	// MUL
						pas[sp + 1] = pas[sp + 1] * pas[sp];
						sp++;
						
						printf("%-9s", "MUL");
						break;
					
					case(4):	// DIV
						pas[sp + 1] = pas[sp + 1] / pas[sp];
						sp++;
						
						printf("%-9s", "DIV");
						break;
					
					case(5):	// EQL
						pas[sp + 1] = pas[sp + 1] == pas[sp];
						sp++;
							
						printf("%-9s", "EQL");
						break;
					
					case(6):	// NEQ
						pas[sp + 1] = pas[sp + 1] != pas[sp];
						sp++;
							
						printf("%-9s", "NEQ");
						break;
					
					case(7):	// LSS
						pas[sp + 1] = pas[sp + 1] < pas[sp];
						sp++;
							
						printf("%-9s", "LSS");
						break;
					
					case(8):	// LEQ
						pas[sp + 1] = pas[sp + 1] <= pas[sp];
						sp++;
							
						printf("%-9s", "LEQ");
						break;
					
					case(9):	// GTR
						pas[sp + 1] = pas[sp + 1] > pas[sp];
						sp++;
							
						printf("%-9s", "GTR");
						break;
					
					case(10):	// GEQ
						pas[sp + 1] = pas[sp + 1] >= pas[sp];
						sp++;
							
						printf("%-9s", "GEQ");
						break;

					case(11):	// ODD	(Unknown instruction, implement modulo for now)
						pas[sp + 1] = pas[sp + 1] % pas[sp];
						sp++;

						printf("%-9s", "ODD");
						break;
				}

				break;
			
			/* Load value to the top of the stack at offset M from L lexicogrphical levels down */
			case(3):	// LOD L,M	
				sp--;
				pas[sp] = pas[base(bp, ir.l) - ir.m];
							
				printf("%-9s", "LOD");
				break;

			/* Store value at top of stack in the stack location at offset M from L lexicographical levels down */			
			case(4):	// STO L,M
				pas[base(bp, ir.l) - ir.m] = pas[sp];
				sp++;
							
				printf("%-9s", "STO");
				break;
			
			/* Call procedure at code index M */
			case(5):	// CAL L,M	
				pas[sp - 1]	= base(bp, ir.l);
				pas[sp - 2] = bp;
				pas[sp - 3] = pc;
				bp = sp - 1;
				pc = ir.m;	
							
				dll++;
				printf("%-9s", "CAL");
				break;
			
			/* Allocate M words to the stack. (Minimum 3 for Static Link, Dynamic Link, and Return Address) */
			case(6):	// INC 0,M	
				sp -= ir.m;
							
				printf("%-9s", "INC");
				break;
			
			/* Jump to instruction M */
			case(7):	// JMP 0,M
				pc = ir.m;
							
				printf("%-9s", "JMP");
				break;
			
			/* Jump to instruction M if top stack element is 0 */
			case(8):	// JPC 0,M
				
				if (pas[sp] == 0) {
					pc = ir.m;
				}
				sp++;
							
				printf("%-9s", "JPC");
				break;
			
			/* System Calls */
			case(9):	// SYS 0,M
				
				// Pop top of stack and output to screen
				if (ir.m == 1) {
					printf("Output result is: %d\n", pas[sp]);
					sp++;
				}
				
				// Read in an integer and store it on top of the stack
				else if (ir.m == 2) {
					sp--;
					printf("Please enter an integer: ");			
					scanf("%d", &pas[sp]);
				}

				// Halt the program
				else {
					halt = 1;
				}
							
				printf("%-9s", "SYS");
				break;
		}

	// Print
	printValues();
	
	}

	return 0;
}
