# Assigment 2: Lexical Analyzer 

## Description
This program takes an input program, written in PL/0, tokenizes the program
and displays the source program, lexeme table, and token list.

## Complilation Instructions
How to compile the program:

'''bash
# Example for a C/C++ Project
gcc lex.c -o lex
'''


## Usage
How to run the compiled program:

'''bash
# Example Usage
./a.out [input_file]
'''


## Example

'''bash
#Example command and expected output
./virtual_machine input.txt

Output:

Source Program:
var x, y;
begin
y := 3;
x := y + 56;
end.

Lexeme Table:
Lexeme              Token Type
var                 29
x                   2
,                   17
y                   2
;                   18
begin               21
y                   2
:=                  20
3                   3
;                   18
x                   2
:=                  20
y                   2
+                   4
56                  3
;                   18
end                 22
.                   19

Token List:
29 2 x 17 2 y 18 21 2 y 20 3 3 18 2 x 20 2 y 4 3 56 18 22 19

## Contact Information

For any questions or issues, please contact:

Garrett Miller
ga404659@ucf.edu

