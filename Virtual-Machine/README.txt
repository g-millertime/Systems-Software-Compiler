# Homework 1: PM/0 Virtual Machine

## Description
This program is a virtual machine representation of a simple stack machine. The program interprets a
series of instructions, provided by an input file, processes the instructions and displays each instruction,
the current value of the program counter, the base pointer for the current activation record, the stack
pointer and the values on the stack after executing each instruction.


## Complilation Instructions
How to compile the program:

'''bash
# Example for a C/C++ Project
gcc hw1.c -o virtual_machine
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
                PC      BP      SP      Stack
Initial values: 10      499     500

  JMP 0 85      85      499     500
  INC 0 5       88      499     495     0 0 0 0 0 
  LIT 0 4       91      499     494     0 0 0 0 0 4 
  STO 0 4       94      499     495     0 0 0 0 4 
  .
  .
  .
  SYS 0 3       106     499     495     0 0 0 24 0  
'''


## Contact Information

For any questions or issues, please contact:

Garrett Miller
ga404659@ucf.edu


