# System Software Projects

This repository contains implementations of various system software components and programs related to assemblers, loaders, and linkers. Each project simulates a specific system software function, offering practical insights into low-level programming concepts.

## Project List

| Folder                     | Description                                                                 |
|----------------------------|-----------------------------------------------------------------------------|
| **Absolute_Loader**        | Loads object code into memory without modifications.                        |
| **Assembler_Pass1_Pass2**  | Two-pass assembler that processes assembly code, creates symbol tables, and generates object code. |
| **EQU**                    | Handles the EQU assembler directive for assigning constant values to symbols. |
| **EQU_Literal**            | Extends assembler functionality to support EQU directives and literals.      |
| **One_Pass_Assembler**     | Processes assembly code and generates object code in a single pass.         |
| **Relocating_Loader**      | Adjusts object code to be loaded at different memory addresses.             |
| **SIC_XE_Assembler_Basic** | Basic assembler for the SIC/XE architecture.                               |
| **Linking_Loader_Pass1_Pass2** | Links multiple object files and loads them into memory in two passes.   |
| **Literals_Updated**       | Handles literals in assembly code.                                         |

## Prerequisites

- A C compiler (e.g., `gcc`) for projects written in C.
- Basic understanding of system software concepts like assemblers and loaders.
- Git installed to clone the repository.

## How to Run

1. **Clone the repository**:
   ```bash
   git clone https://github.com/FardeenVaddo/system-software-projects.git
   cd system-software-projects
   ```
