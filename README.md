System Software Projects
This repository contains implementations of various system software components and programs related to assemblers, loaders, and linkers. Each folder includes a program that simulates a specific system software function.

Project List

Folder | Description
Absolute Loader | Program that loads object code into memory without modifications.
Assembler_pass1_pass2 | Two-pass assembler that processes assembly code, creates symbol tables, and generates object code.
EQU | Program that handles the EQU assembler directive for assigning constant values to symbols.
Equ_literal | Extends assembler functionality to handle both EQU directives and literals.
One pass Assembler | Assembler implementation that processes code and generates output in a single pass.
Relocating_loader | Loader program that adjusts object code so it can be loaded at different memory addresses.
SIC-XE-Assembler-basic | Simple assembler program for the SIC/XE architecture.
linking_loader_pass1_pass2 | Linking loader implementation that links multiple object files and loads them into memory in two passes.
literals_updated/literals | Program to handle literals in assembly code.

How to Run
1. Clone the repository:
git clone https://github.com/your-username/system-software-projects.git
cd system-software-projects
2.Navigate to a project folder:
cd Absolute\ Loader
3.Compile and run the code (adjust the command based on the programming language used):
gcc loader.c -o loader
./loader
Author
Fardeen Vaddo
GitHub
