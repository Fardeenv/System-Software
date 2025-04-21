#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define MAX_INSTRUCTIONS 50

struct
{
    char label[20], opcode[20], operand[20];
    unsigned long addr;
} INSTR[MAX_INSTRUCTIONS];

struct
{
    char symbols[20];
    int addr;
} SYMTAB[20];


struct
{
    char mnemonic[8];
    int length;
    unsigned char opcode;
} OPTAB[30] = {
    {"LDA", 3, 0x00}, {"STA", 3, 0x0C}, {"LDX", 3, 0x04}, {"STX", 3, 0x10}, {"ADD", 3, 0x18},
    {"SUB", 3, 0x1C}, {"MUL", 3, 0x20}, {"DIV", 3, 0x24}, {"COMPR", 2, 0xA0}, {"JEQ", 3, 0x30},
    {"JGT", 3, 0x34}, {"JLT", 3, 0x38}, {"J", 3, 0x3C}, {"AND", 3, 0x40}, {"OR", 3, 0x44},
    {"JSUB", 3, 0x48}, {"RSUB", 3, 0x4C}, {"LDL", 3, 0x08}, {"STL", 3, 0x14}, {"ADDR", 2, 0x90},
    {"LDS", 3, 0x6C}, {"LDT", 3, 0x74}, {"TIX", 3, 0x2C}, {"TIXR", 2, 0xB8}, {"LDCH", 3, 0x50}, {"STCH", 3, 0x54}
};

unsigned long pc = 0x0000;
unsigned long start_addr = 0x0000;
int len = 0;
int read_count = 0;
int lit_count = 0;
int sym_count = 0;

void insert_into_symtab(FILE *symtab, char label[100], unsigned long pc)
{
    fprintf(symtab, "%s %lX\n", label, pc);
}

int search_symtab(int count, char label[100])
{
    for (int j = 0; j < count; j++)
    {
        if ((!strcmp(label, SYMTAB[j].symbols)) && (SYMTAB[j].symbols[0] != '_'))
        {
            return 1;
        }
    }
    return 0;
}

int search_optab(char opcode[100], int *length)
{
    for (int i = 0; OPTAB[i].length != 0; i++)
    {
        if (!strcmp(opcode, OPTAB[i].mnemonic))
        {
            *length = OPTAB[i].length;
            return 1;
        }
    }
    return 0;
}


void pass1()
{
    FILE *symtab = fopen("symtab.txt", "w");
    if (symtab == NULL)
    {
        perror("Error opening symtab.txt");
        exit(1);
    }

    FILE *f = fopen("equ.txt", "r");
    if (f == NULL)
    {
        perror("Error opening instruction.txt");
        fclose(symtab);
        exit(1);
    }

    FILE *output = fopen("intermediate.txt", "w");
    if (output == NULL)
    {
        perror("Error opening intermediate.txt");
        fclose(symtab);
        fclose(f);
        exit(1);
    }

    char line[50];
    while (fgets(line, sizeof(line), f))
    {
        char label[100] = "", opcode[100] = "", operand[100] = "", temp[100] = "";

        int numFields = sscanf(line, "%s %s %s", label, opcode, operand);

        if (numFields <= 0)
            continue;

        if (numFields == 2)
        {
            strcpy(temp, opcode);
            strcpy(opcode, label);
            strcpy(operand, temp);
            strcpy(label, "_");
        }

        if (!strcmp(opcode, "START"))
        {
            start_addr = strtoul(operand, NULL, 16);
            pc = start_addr;
            printf("Starting address: %X\n", pc);
            if (strcmp(label, "_") != 0)
            {
                insert_into_symtab(symtab, label, pc);
                sym_count++;
            }
            fprintf(output, "%lX %s %s %s\n", pc, label, opcode, operand);
            continue;
        }

        if (!strcmp(opcode, "EQU"))
        {
            unsigned long equ_value = 0;
            int symbol_found = 0;

            // Operand is a numeric constant
            if (isdigit(operand[0]) || operand[0] == '-')
            {
                equ_value = strtol(operand, NULL, 10); // Decimal value
            }
            // Operand is a hexadecimal constant prefixed by 'X'
            else if (operand[0] == 'X' && operand[1] == '\'')
            {
                equ_value = strtoul(operand + 2, NULL, 16); // Hex value inside quotes
            }
            // Operand references another symbol
            else
            {
                for (int k = 0; k < sym_count; k++)
                {
                    if (!strcmp(SYMTAB[k].symbols, operand))
                    {
                        equ_value = SYMTAB[k].addr;
                        symbol_found = 1;
                        break;
                    }
                }
                if (!symbol_found)
                {
                    printf("Error: Undefined symbol '%s' in EQU\n", operand);
                    break;
                }
            }

            // Record the EQU symbol in SYMTAB if it has a label
            if (strcmp(label, "_") != 0)
            {
                if (search_symtab(sym_count, label))
                {
                    printf("Error: Duplicate symbol '%s'\n", label);
                    break;
                }
                strcpy(SYMTAB[sym_count].symbols, label);
                SYMTAB[sym_count].addr = equ_value;
                insert_into_symtab(symtab, label, equ_value);
                sym_count++;
            }

            fprintf(output, "%lX %s %s %s\n", pc, label, opcode, operand);
            continue;
        }


        INSTR[read_count].addr = pc;
        strcpy(INSTR[read_count].label, label);
        strcpy(INSTR[read_count].opcode, opcode);
        strcpy(INSTR[read_count].operand, operand);

        if (strcmp(opcode, "END") != 0)
        {
            if (strcmp(label, "_") != 0)
            {
                int flag = search_symtab(read_count, label);
                if (flag)
                {
                    printf("Error: Duplicate symbol '%s'\n", label);
                    break;
                }
                strcpy(SYMTAB[read_count].symbols, label);
                insert_into_symtab(symtab, label, pc);
                sym_count++;
            }

            int instr_length = 0;
            if (search_optab(opcode, &instr_length))
            {
                pc += instr_length;
            }
            else if (!strcmp(opcode, "WORD"))
            {
                pc += 3;
            }
            else if (!strcmp(opcode, "RESW"))
            {
                pc += 3 * atoi(operand);
            }
            else if (!strcmp(opcode, "RESB"))
            {
                pc += atoi(operand);
            }
            else if (!strcmp(opcode, "BYTE"))
            {
                if (operand[0] == 'C')
                {
                    pc += strlen(operand) - 3;
                }
                else if (operand[0] == 'X')
                {
                    pc += (strlen(operand) - 3) / 2;
                }
                else if (operand[0] == 'O')
                {
                    pc += ceil(((strlen(operand) - 3) * 3) / 8.0);
                }
            }
            else
            {
                printf("Error: Invalid opcode '%s'\n", opcode);
                continue;
            }
            fprintf(output, "%lX %s %s %s\n", INSTR[read_count].addr, label, opcode, operand);
            read_count++;
        }
        else
        {
            break;
        }
    }

    fclose(f);
    fclose(output);
    fclose(symtab);
}

int main()
{
    pass1();
    return 0;
}
