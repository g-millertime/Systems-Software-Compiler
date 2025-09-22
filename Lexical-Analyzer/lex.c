/* Assignment 2: Lexigraphic Analyzer
 * Garrett Miller
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LENGTH 11
#define MAX_DIGITS 5

char * keywords[14] = {"const", "var", "procedure", "call", "begin", "end",
					"if", "fi", "then", "else", "while", "do", "read", "write"};

char specialsym[13] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<','>', ';', ':'};

typedef enum {error = 0, nulsym, identsym, numbersym, plussym, minussym, 
			multsym, slashsym, fisym, eqlsym, neqsym, lessym, leqsym,
			gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
			periodsym, becomesym, beginsym, endsym, ifsym, thensym,
			whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
			readsym, elsesym} token_type;	

typedef struct LexemeTable LexTab;

struct LexemeTable {
	char ** lexeme;
	int * token;
	int size, cap;
};

LexTab * initTable() {
	LexTab * res = (LexTab *) malloc(sizeof(LexTab));
	
	res->size = 0;
	res->cap = 1;
	res->lexeme = (char **) calloc(res->cap, sizeof(char *));
	res->token = (int *) calloc(res->cap, sizeof(int));
	
	return res;
	
}

/* Add a lexeme to the Lexeme Table */
void addLexeme(LexTab * tab, char ** word, token_type * tok, int * len) {
	// Resize if table doesn't have enough space
	if (tab->size == tab->cap) {
		tab->cap *= 2;
		tab->lexeme = (char **) realloc(tab->lexeme, sizeof(char *) * tab->cap);
		tab->token = (int *) realloc(tab->token, sizeof(int) * tab->cap);
	}

	// Add lexeme and token
	tab->lexeme[tab->size] = strdup(*word);
	tab->token[tab->size++] = *tok;

	// Reset values for word and tok
	memset(*word, 0, strlen(*word));
	*tok = 0;
	*len = 0;

}

/* Print all lexemes and tokens in the table */
void printLexemeTable(LexTab * tab) {	
	printf("\nLexeme Table:\n");
	printf("%-20s%s\n", "Lexeme", "Token Type");
	for (int i = 0; i < tab->size; i++) {
		if (tab->token[i] == 0)	
			printf("%-20s%s\n", tab->lexeme[i], "error: token not found");
		else
			printf("%-20s%d\n", tab->lexeme[i], tab->token[i]);
	}
	printf("\n");
}

/* Print all tokens, identifiers, and integers */
void printTokenList(LexTab * tab) {
	printf("Token List:\n");
	for (int i = 0; i < tab->size; i++) {
		// Ignore error tokens
		if (tab->token[i] == 0) continue;
		
		printf("%d ", tab->token[i]);
		
		// Check for idenifier or digit
		if (tab->token[i] == 2 || tab->token[i] == 3) {
			printf("%s ", tab->lexeme[i]);
		}
	}
	printf("\n");
}

void printSource(char * f) {
		FILE * file = fopen(f, "r");
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
	for (int i = 0; i < 13; i++) {
		if (c == specialsym[i]) {
			return i;
		}
	}
	return -1;
}

/* Checks for matching keyword, returns index of keyword */
int isKeyword(char * word) {
	for (int i = 0; i < 14; i++) {
		if (strcmp(word, keywords[i]) == 0) 
			return i;
	}
	return -1;
}

/* Main program */
int main(int argc, char ** argv) {
	char * word = (char *) calloc(20, sizeof(char));
	int len = 0;
	int index;
	token_type tok;
	LexTab * lexTab = initTable();

	printSource(argv[1]);
	
	/* Retrieve characters individually from file and process */
	FILE * input = fopen(argv[1], "r");
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
					case -1:
						tok = error;
						break;
					case 0:
						tok = constsym;
						break;
					case 1:
						tok = varsym;
						break;
					case 2:
						tok = procsym;
						break;
					case 3:
						tok = callsym;
						break;
					case 4:
						tok = beginsym;
						break;
					case 5:
						tok = endsym;
						break;
					case 6:
						tok = ifsym;
						break;
					case 7:
						tok = fisym;
						break;
					case 8:
						tok = thensym;
						break;
					case 9:
						tok = elsesym;
						break;
					case 10:
						tok = whilesym;
						break;
					case 11:
						tok = dosym;
						break;
					case 12:
						tok = readsym;
						break;
					case 13:
						tok = writesym;
						break;
				}
			}
			else {
				// Check that current word is correct length
				if (strlen(word) > MAX_LENGTH)
					tok = error;
				else 
					tok = identsym;
			}

			// Add token to lexeme table
			addLexeme(lexTab, &word, &tok, &len);

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
			if (len > MAX_DIGITS) tok = error;

			addLexeme(lexTab, &word, &tok, &len);		
		}

		if ((index = isSym(c)) != -1) {
			
			// Add to current word for lexeme
			word[len++] = c;

			switch (index) {
				case 0:	// +
					tok = plussym;
					break;
				case 1:	// -
					tok = minussym;
					break;
				case 2:	// *
					tok = multsym;
					break;
				case 3:	// /
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
				case 4:	// (
					tok = lparentsym;
					break;
				case 5:	// )
					tok = rparentsym;
					break;
				case 6:	// =
					tok = eqlsym;
					break;
				case 7:	// ,
					tok = commasym;
					break;
				case 8:	// .
					tok = periodsym;
					break;
				case 9:	// <
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
				case 10:	// >
					// check for '>='
					c = fgetc(input);
					if (c == '=') {
						word[len++] = c;
						tok = geqsym;
					}
					else					
						tok = gtrsym;
					break;
				case 11:	// ;
					tok = semicolonsym;
					break;
				case 12:	// :=, invalid if only :
					if ((c = fgetc(input)) == '=') {
						word[len++] = c;
						tok = becomesym;
					}
					else
						tok = error;
					break;	
			}
			
			if (strcmp(word, "") != 0) {
				addLexeme(lexTab, &word, &tok, &len);
			}

		}

		// If Lexeme is processed get next character
		if (!isalpha(c) && !isdigit(c)) 
				c = fgetc(input);
	}	
	
	// Print Lexeme Table
	printLexemeTable(lexTab);

	// Print Token List
	printTokenList(lexTab);


	fclose(input);
}
