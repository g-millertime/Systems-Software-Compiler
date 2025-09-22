# Systems Software PL/0 Virtual Machine and Compiler
This project creates a virtual machine, lexical analyzer, and complier for a small language 
called PL/0. The compiler uses recurive descent parsing from the grammar defined in Extended 
Backus-Naur Form (EBNF) below.

## Instruction Set Architecture (ISA)
In the following tables, italicized names (such as p) are meta-variables that refer to integers.
If an instruction’s field is notated as “-“, then its value does not matter (we use 0 as a
placeholder for such values in examples).

| Code | Instruction | Description |
| ---- | ----------- | ----------- |
| 01 | LIT | 0, M Pushes a constant value (literal) M onto the stack |
| 02 | OPR | 0, M Operation to be performed on the data at the top of the stack (or return from function) |
| 03 | LOD | L, M Load value to top of stack from the stack location at offset M from L lexicographical levels down |
| 04 | STO | L, M Store value at top of stack in the stack location at offset M from L lexicographical levels down |
| 05 | CAL | L, M Call procedure at code index M (generates new Activation Record and PC  M) |
| 06 | INC | 0, M Allocate M memory words (increment SP by M). First three are reserved to Static Link (SL), Dynamic Link (DL), and Return Address (RA) |
| 07 | JMP | 0, M Jump to instruction M (PC  M) |
| 08 | JPC | 0, M Jump to instruction M if top stack element is 0 |
| 09 | SYS | 0, 1 Write the top stack element to the screen |
| 09 | SYS | 0, 2 Read in input from the user and store it on top of the stack |
| 09 | SYS | 0, 3 End of program (Set “eop” flag to zero |

## Extended Backus-Naur Form (EBNF)
- program ::= block "." .
- block ::= const-declaration var-declaration procedure-declaration statement.
- constdeclaration ::= ["const" ident "=" number {"," ident "=" number} ";"].
- var-declaration ::= [ "var "ident {"," ident} “;"].
- procedure-declaration ::= { "procedure" ident ";" block ";" }
- statement ::= [ ident ":=" expression | "call" ident | "begin" statement { ";" statement }
"end" | "if" condition "then" statement "fi" | "while" condition "do" statement | "read" ident
| "write" expression | empty ] .
- condition ::= "odd" expression | expression rel-op expression.
- rel-op ::= "="|“< >"|"<"|"<="|">"|">=“.
- expression ::= term { ("+"|"-") term}.
- term ::= factor {("*"|"/") factor}.
- factor ::= ident | number | "(" expression ")“.
- number ::= digit {digit}.
- ident ::= letter {letter | digit}.
- digit ;;= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9“.
- letter ::= "a" | "b" | ... | "y" | "z" | "A" | "B" | ... | "Y" | "Z".
