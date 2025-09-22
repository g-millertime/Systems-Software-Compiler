/* Assignment 3: Tiny PL/0 Compiler
 * Garrett Miller
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#include "./parsercodegen.h"
#define MAX_LENGTH 11
#define MAX_DIGITS 5
#define NUM_KEYWORDS 11
#define NUM_SYMBOLS 13
#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_TEXT_SPACE 100

typedef struct LexemeTable LexemeTable;
typedef struct SymbolTable SymbolTable;
typedef struct Text Text;
typedef enum {  oddsym = 1, identsym, numbersym, plussym, minussym,
                multsym, slashsym, fisym, eqlsym, neqsym,
                lessym, leqsym, gtrsym, geqsym, lparentsym,
                rparentsym, commasym, semicolonsym, periodsym, becomesym,
                beginsym, endsym, ifsym, thensym, whilesym,
                dosym, callsym, constsym, varsym, procsym,
                writesym, readsym, elsesym} token_type;

struct LexemeTable {
    char ** lexeme;
    int * token;
    int size, cap;
};

struct SymbolTable {
    int kind;
    char name[MAX_LENGTH + 1];
    int value;
    int level;
    int address;
    int mark;
};

struct Text {
    int op;
    int L;
    int M;
};

LexemeTable * initLexemeTable();
void addLexeme(char ** word, token_type * tok, int * len);
void printLexemeTable();
void printTokenList();
void printSource(char * filename);
int isSym(char c);
int isKeyword(char * word);
void scanner(char * filename);
int checkSymbolTable(char * name);
void addSymbol(int kind, char * name, int value, int level, int address, int mark);
void markSymbolTable(int x);
void printSymbolTable();
void writeELF(char * filename);
void error(int x);
void emit(int op, int L, int M);
int syntaxanalyzer();
void constantdeclaration();
int variabledeclaration();
void statement();
void condition();
void expression();
void term();
void factor();
void printText();
char * getOP(int i);
char * keywords[12] = {"const", "var", "begin", "end",
					"if", "fi", "then", "while", "do", "read", "write", "odd"};

char specialsym[13] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<','>', ';', ':'};

// Global Lexeme Table
LexemeTable * lexTab;
int ltx = 0;

// Global Symbol Table
SymbolTable symTab[MAX_SYMBOL_TABLE_SIZE];
int stsize = 0;

// Global Text Array 
Text text[MAX_TEXT_SPACE];
int cx = 0;

/* Initialize Lexeme Table */
LexemeTable * initLexemeTable() {
	LexemeTable * res = (LexemeTable *) malloc(sizeof(LexemeTable));
	
	res->size = 0;
	res->cap = 1;
	res->lexeme = (char **) calloc(res->cap, sizeof(char *));
	res->token = (int *) calloc(res->cap, sizeof(int));
	
	return res;
	
}

/* Add a lexeme to the Lexeme Table */
void addLexeme(char ** word, token_type * tok, int * len) {
	// Resize if lexTable doesn't have enough space
	if (lexTab->size == lexTab->cap) {
		lexTab->cap *= 2;
		lexTab->lexeme = (char **) realloc(lexTab->lexeme, sizeof(char *) * lexTab->cap);
		lexTab->token = (int *) realloc(lexTab->token, sizeof(int) * lexTab->cap);
	}

	// Add lexeme and token
	lexTab->lexeme[lexTab->size] = strdup(*word);
	lexTab->token[lexTab->size++] = *tok;

	// Reset values for word and tok
	memset(*word, 0, strlen(*word));
	*tok = 0;
	*len = 0;

}

/* Print all lexemes and tokens in the lexTable */
void printLexemeTable() {	
	printf("\nLexeme Table:\n");
	printf("%-20s%s\n", "Lexeme", "Token Type");
	printf("-------------------------------------\n");
	for (int i = 0; i < lexTab->size; i++) {
		if (lexTab->token[i] == 0)	
			printf("%-20s%s\n", lexTab->lexeme[i], "Error: Token not found.");
		else
			printf("%-20s%d\n", lexTab->lexeme[i], lexTab->token[i]);
	}
	printf("\n");
}

/* Print all tokens, identifiers, and integers */
void printTokenList() {
	printf("Token List:\n");
	for (int i = 0; i < lexTab->size; i++) {
		// Ignore error tokens
		if (lexTab->token[i] == 0) continue;
		
		printf("%d ", lexTab->token[i]);
		
		// Check for idenifier or digit
		if (lexTab->token[i] == 2 || lexTab->token[i] == 3) {
			printf("%s ", lexTab->lexeme[i]);
		}
	}
	printf("\n\n");
}

/* Print Source File */
void printSource(char * filename) {
		FILE * file = fopen(filename, "r");
		char c;

		printf("Source Program: \n");
		while ((c = fgetc(file)) != EOF) {
			printf("%c", c);
		}

		fclose(file);
}

/* Check that a character is in the special symbol list 
 * Returns index of symbol or 0 if not found. */
int isSym(char c) {
	for (int i = 0; i < NUM_SYMBOLS; i++) {
		if (c == specialsym[i]) {
			return i;
		}
	}
	return -1;
}

/* Checks for matching keyword, returns index of keyword */
int isKeyword(char * word) {
	for (int i = 0; i < NUM_KEYWORDS; i++) {
		if (strcmp(word, keywords[i]) == 0) 
			return i;
	}
	return -1;
}

/* Scanner */
void scanner(char * filename) {
	char * word = (char *) calloc(20, sizeof(char));
	int len = 0;
	int index;
	token_type tok;

	/* Retrieve characters individually from file and process */
	FILE * input = fopen(filename, "r");
	char c = fgetc(input);
	while (c != EOF) {	
		
		// Character is a letter, loop until next character is a special symbol or whitespace
		if (isalpha(c)) {
	
			// Add to word until next character is whitespace, a symbol or EOF
			while (isalnum(c)) {
				word[len++] = c;
				c = fgetc(input);
			}
			
			// Check for matching keyword
			if ((index = isKeyword(word)) != -1) {		
				switch (index) {
					case 0: tok = constsym; break;
					case 1: tok = varsym; break;
					case 2: tok = beginsym; break;
					case 3: tok = endsym; break;
					case 4: tok = ifsym; break;
					case 5:	tok = fisym; break;
					case 6: tok = thensym; break;
					case 7: tok = whilesym; break;
					case 8:	tok = dosym; break;
					case 9: tok = readsym; break;
					case 10: tok = writesym; break;
					case 11: tok = oddsym; break;
				}
			}
			else {
				// Check that current word is correct length
				if (strlen(word) > MAX_LENGTH)
					error(17);
				else 
					tok = identsym;
			}

			// Add token to lexeme Lexeme Table
			addLexeme(&word, &tok, &len);

		}

		// Lexeme is a number
		if (isdigit(c)) {
			tok = numbersym;	
			
			// 	Continue checking for more numbers
			while (isdigit(c)) {
				word[len++] = c;
				c = fgetc(input);
			}
			
			// Check that digit is appropriate length
			if (len > MAX_DIGITS) error(18);

			addLexeme(&word, &tok, &len);		
		}

		if ((index = isSym(c)) != -1) {
			
			// Add to current word for lexeme
			word[len++] = c;

			switch (index) {
				case 0:	tok = plussym; break;
				case 1:	tok = minussym;	break;
				case 2:	tok = multsym; break;

				// /
				case 3:	
					tok = slashsym;
					// Check for * for comment
					c = fgetc(input);
					if (c == '*') {	
						// Backtrack current word, since we want to ignore comments
						word[--len] = '\0';
						// Continue checking for "*/" until found or EOF
						while ((c = fgetc(input)) != EOF) {
							if (c == '*' && (c = fgetc(input)) == '/') 
								break;
						}
					}
					break;

				case 4:	tok = lparentsym; break;
				case 5:	tok = rparentsym; break;
				case 6:	tok = eqlsym; break;
				case 7:	tok = commasym; break;
				case 8:	tok = periodsym; break;
				
				// <
				case 9:	
					// check for '<>'
					c = fgetc(input);
					
					if (c == '>') {
						word[len++] = c;
						tok = neqsym;
					}
					
					// check for '<='
					else if (c == '=') {
						word[len++] = c;
						tok = leqsym;
					}
					
					else
						tok = lessym;	
					break;
				
				// >
				case 10:
					// check for '>='
					c = fgetc(input);
					if (c == '=') {
						word[len++] = c;
						tok = geqsym;
					}
					else					
						tok = gtrsym;
					break;
				
				case 11: tok = semicolonsym; break;
				
				// :
				case 12:	
					//invalid if only :
					if ((c = fgetc(input)) == '=') {
						word[len++] = c;
						tok = becomesym;
					}
					else
						error(15);
					break;	
			}
		
			// Check that current token is not-empty, add to table if npt.	
			if (strcmp(word, "") != 0) {
				addLexeme(&word, &tok, &len);
			}

		}

		// If Lexeme is processed get next character
		if (!isalpha(c) && !isdigit(c)) 
				c = fgetc(input);
	}	
	
	
	fclose(input);
}

/* Add symbol to Symbol Table */
void addSymbol(int kind, char * name, int value, int level, int address, int mark) {
	symTab[stsize].kind = kind;
	strcpy(symTab[stsize].name, name);
	symTab[stsize].value = value;
	symTab[stsize].level = level;
	symTab[stsize].address = address;
	symTab[stsize].mark = mark;
}

/* Check if token in symbol table, return index or -1 */
int checkSymbolTable(char * name) {
	for (int i = 0; i < stsize; i++) {
		if (strcmp(name, symTab[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

/* Mark symbol table */
void markSymbolTable(int L) {
	for (int i = 0; i < stsize; i++) {
		if (symTab[i].level == 0) {
			symTab[i].mark = 1;
		}
	}
}

/* Print symbol table */
void printSymbolTable() {
	printf("%4s | %11s | %5s | %5s | %7s | %4s\n",
		   "Kind", "Name", "Value", "Level", "Address", "Mark");
	printf("---------------------------------------------------\n");	
	
	for (int i = 0; i < stsize; i++) {
		printf("%4d | %11s | %5d | %5d | %7d | %4d\n",
				symTab[i].kind,
				symTab[i].name,
				symTab[i].value,
				symTab[i].level,
				symTab[i].address,
				symTab[i].mark);
				
	}
	printf("\n");
}

/* Display error messages and terminate program upon error. */
void error(int x) {
	printf("Error: ");

	switch (x) {
		case 1: printf("program must end with a period.\n"); break;
		case 2: printf("const, var, and read keywords must be followed by an identifier.\n"); break;
		case 3:	printf("symbol name has already been declared.\n"); break;
		case 4: printf("constants must be assigned with =.\n"); break;
		case 5: printf("constants must be assigned an integer value.\n"); break;
		case 6:	printf("constant and variable declarations must be followed by a semicolon.\n"); break;
		case 7:	printf("undeclared identifier.\n");	break;
		case 8:	printf("only variable values may be altered.\n"); break;
		case 9:	printf("assignment statements must use :=\n"); break;
		case 10: printf("begin must be followed by end.\n"); break;
		case 11: printf("if must be followed by then.\n"); break;
		case 12: printf("while must be followed by do.\n");	break;
		case 13: printf("condition must contain comparison operator.\n"); break;
		case 14: printf("right parenthisis must follow left parenthesis.\n"); break;
		case 15: printf("arithmetic equations must contain operands, parenthesis, numbers or symbols.\n"); break;
		case 16: printf("then must be followed by fi.\n"); break;
		case 17: printf("identifiers must be 11 characters or less.\n"); break;
		case 18: printf("numbers must be less than 5 digits.");
	}

	exit(0);
}

/* Generate instruction */
void emit(int op, int L, int M) {
	/*
	if (cx > CODE_SIZE) {
		error(25);
	}
	*/

	text[cx].op = op;
	text[cx].L = L;
	text[cx].M = M;
	cx++;

}

/* Parse token list for syntax */
int syntaxanaylzer() {
	
	/* Program Start */
	// emit inital jump instuction
	emit(7, 0, 3);

	/* Constant Declaration */
	if (lexTab->token[ltx] == constsym) {
		constantdeclaration();
	}
		
	/* Variable Declaration */
	if (lexTab->token[ltx] == varsym) {
		int numvars = variabledeclaration();	
		emit(6, 0, numvars + 3);	// INC
	}

	/* Statement */
	statement();

	// Check for period at End of Program	
	if (lexTab->token[ltx] != periodsym) {
		error(1);
	}

	/* Program End */
	// Emit system halt instruction
	emit(9, 0, 3); // HALT

	// Mark symbol table (all values at same level)
	markSymbolTable(0);

	return 0;
}

/* Parse constant declaration */
void constantdeclaration() {	
	char * name;
	
	do {
		ltx++;

		// Check identifier
		if (lexTab->token[ltx] != identsym) {
			error(2);
		}
		
		// Save identifier name
		name = lexTab->lexeme[ltx];
		ltx++;

		// Check assignment with =
		if (lexTab->token[ltx++] != eqlsym) {
			error(4);
		}

		// Check for value
		if (lexTab->token[ltx] != numbersym) {
			error(5);
		}
		
		// Add lexeme to Symbol Table
		addSymbol(1, name, atoi(lexTab->lexeme[ltx++]), 0, 0, 0);
		stsize++;
		
		// Continue while there is a comma
	} while (lexTab->token[ltx] == commasym);

	// Check semicolon
	if (lexTab->token[ltx++] != semicolonsym) {
		error(6);
	}
}

/* Parse variable declaration */
int variabledeclaration() {
	int numvars = 0;
	int index;

	do {
		numvars++;
		ltx++;
		
		// Check identifier
		if (lexTab->token[ltx] != identsym) {
			error(2);
		}

		// Check if already in symbol table
		index = checkSymbolTable(lexTab->lexeme[ltx]);
		if (index != -1) error(3);	
		// Add lexeme to Symbol Table
		addSymbol(2, lexTab->lexeme[ltx], 0, 0, numvars + 2, 0);
		stsize++;

		ltx++;

		// Continue while there is a comma
	} while (lexTab->token[ltx] == commasym);

	// Check semicolon
	if (lexTab->token[ltx++] != semicolonsym) {
		error(6);
	}

	return numvars;
}

/* Parse statement declaration */
void statement() {
	int index, address, jpcx, loopx;
	
	switch (lexTab->token[ltx]) {
		case identsym:
			// Check if symbol in table
			index = checkSymbolTable(lexTab->lexeme[ltx]);
			ltx++;
			if (index == -1) error(7);
			
			// Check that symbol is a variable
			if (symTab[index].kind != 2) error(8);
			
			// Check for assigment
			if (lexTab->token[ltx++] != becomesym ) {
				error(9);
			}

			expression();
			emit(4, 0, symTab[index].address);	// STO
			return;

		case beginsym:
			do {
				ltx++;
				statement();
			} while (lexTab->token[ltx] == semicolonsym);
			
			if (lexTab->token[ltx++] != endsym) {
				error(10);
			}
			return;
			
		case ifsym:
			ltx++;
			condition();
			
			// Add jump instuction, mark current position for
			// later update
			jpcx = cx;
			emit(8, 0, jpcx * 3); // JPC
			
			// Check "then" follows "if"	
			if (lexTab->token[ltx++] != thensym) {
				error(11);
			}
			
			statement();

			// Update jump conditional index 
			text[jpcx].M = cx * 3;

			if (lexTab->token[ltx++] != fisym) {
				error(16);
			}
			return;

		case whilesym:
			ltx++;
			
			// Mark current code position for loop
			loopx = cx;

			condition();
			
			// Check "do" follows "while"
			if (lexTab->token[ltx++] != dosym) error(12);
			
			// Add Jump Conditional instruction, mark current
			// position for later update.
			jpcx = cx;
			emit(8, 0, jpcx * 3);

			statement();

			// Jump to instruction before conditional
			// evaluation.
			emit(7, 0, loopx * 3);
			
			// Update jump conditional index
			text[jpcx].M = cx * 3;
			return;
			
		case readsym:
			ltx++;
			
			if (lexTab->token[ltx] != identsym) error(2);
			
			index = checkSymbolTable(lexTab->lexeme[ltx++]);
			if (index == -1) error(7);
			if (symTab[index].kind != 2) error(2);
			
			emit(9, 0, 2); // READ
			emit(4, 0, symTab[index].address);	// STO
			return;

		case writesym:
			ltx++;
			expression();
			emit(9, 0, 1); // WRITE
			return;
	}
}

/* Parse condition */
void condition() {
	if (lexTab->token[ltx] == oddsym) {
		expression();
		emit(2, 0, 11); // ODD
	}
	else {
		expression();
		
		switch (lexTab->token[ltx++]) {
			case eqlsym: expression(); emit(2, 0, 5); break;	// EQL
			case neqsym: expression(); emit(2, 0, 6); break;	// NEQ
			case lessym: expression(); emit(2, 0, 7); break;	// LES
			case leqsym: expression(); emit(2, 0, 8); break;	// LEQ
			case gtrsym: expression(); emit(2, 0, 9); break;	// GTR
			case geqsym: expression(); emit(2, 0, 20); break;	// GEQ
			default: error(13); break;
		}
	}
}

/* Parse expression */
void expression() {
	
	term();

	while (lexTab->token[ltx] == plussym || lexTab->token[ltx] == minussym) {
		if (lexTab->token[ltx] == plussym) {
			ltx++;
			term();
			emit(2, 0, 1);	// ADD
		}

		else if (lexTab->token[ltx] == minussym) {
			ltx++;
			term();
			emit(2, 0, 2);	// SUB
		}
	}
}

/* Parse term */
void term() {
	factor();
	
	while (lexTab->token[ltx] == multsym || lexTab->token[ltx] == slashsym) {
		if (lexTab->token[ltx] == multsym) {
			ltx++;
			factor();
			emit(2, 0, 3); // MULT
		}
		else {
			ltx++;
			factor();
			emit(2, 0, 4);	// DIV
		}
	}
}

/* Parse factor */
void factor() {
	int index, value;
	
	switch (lexTab->token[ltx]) {
		case identsym: 
			
			// Check if in symbol table
			index = checkSymbolTable(lexTab->lexeme[ltx++]);
			if (index == -1) error(7);
			// Check if identifier is constant
			if (symTab[index].kind == 1) 
				emit(1, 0, symTab[index].value); // LIT
			else 
				emit(3, 0, symTab[index].address); // LOD
			break;

		case numbersym:
		   emit(1, 0, atoi(lexTab->lexeme[ltx++])); // LIT, 0, M
		   break;

		case lparentsym:
			ltx++;
			expression();
			// Check right parenthesis follows left
			if (lexTab->token[ltx++] != rparentsym) error(14);
			break;
		
		default:
			error(15);
			break;
	}
}

/* Print Text Area */
void printText() {
	printf("%4s %3s %3s %s\n", "Line", "OP", "L", "M");
	for (int i = 0; i < cx; i++) {
		printf("%4d %3s %3d %d\n", i, getOP(i), text[i].L, text[i].M);
	}
	printf("\n");
}

/* Match pneumonic to OP */
char * getOP(int i) {
	switch (text[i].op) {
		case 1: return "LIT";
		case 2:	// OPR 
			switch(text[i].M) {
				case 1: return "ADD";
				case 2: return "SUB";
				case 3: return "MUL";
				case 4: return "DIV";
				case 5: return "EQL";
				case 6: return "NEQ";
				case 7: return "LSS";
				case 8: return "LEQ";
				case 9: return "GTR";
				case 10: return "GEQ";
			}
		case 3: return "LOD";
		case 4: return "STO";
		case 5: return "CAL";
		case 6: return "INC";
		case 7: return "JMP";
		case 8: return "JPC";
		case 9: return "SYS";
	}
}

/* Create excecutable linkable file */
void writeELF(char * filename) {
	FILE * output = fopen(filename, "w");
	
	for (int i = 0; i < cx; i++) {
		fprintf(output, "%d %d %d\n", text[i].op, text[i].L, text[i].M);
	}

	fclose(output);
}


/* PL/0 Tiny Compiler */
int main(int argc, char ** argv) {
	char * input_filename = argv[1],
		 * output_filename = argv[2];
	
	/* IGNORE HW3 *
	printSource(filename);
	*/

	// Initialize Lexeme Table
	lexTab = initLexemeTable();

	// Generate tokens from Scanner
	scanner(input_filename);

	/* IGNORE HW3 *
	// Print Lexeme Table 
	printLexemeTable();

	// Print Token List
	printTokenList();
	*/


	// Parse tokens for syntax
	syntaxanaylzer();

	// Print Assembly Code
	printText();

	// Print Symbol Table	
	printSymbolTable();

	writeELF(output_filename);
		
	return 0;
}
