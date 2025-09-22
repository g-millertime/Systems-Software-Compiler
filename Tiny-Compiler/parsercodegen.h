#define MAX_LENGTH 11
#define MAX_DIGITS 5
#define NUM_KEYWORDS 11
#define NUM_SYMBOLS 13
#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_TEXT_SPACE 100

typedef struct LexemeTable LexemeTable;
typedef struct SymbolTable SymbolTable;
typedef struct Text Text;
typedef enum {	invalidsym = 0,
				oddsym, identsym, numbersym, plussym, minussym,
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
