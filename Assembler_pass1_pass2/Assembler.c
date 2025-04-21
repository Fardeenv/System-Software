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
    char constant[20];  // Store the entire literal as a string
    char type;
} CONSTTAB[20];

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
        if ((!strcmp(label, SYMTAB[j].symbols)) && (SYMTAB[j].symbols[0] !='_'))
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

void display_literals()
{
    printf("CHARACTER LITERALS:\n");
    for (int i = 0; i < lit_count; i++)
    {
        if (CONSTTAB[i].type == 'C')
            printf("%s\n", CONSTTAB[i].constant);
    }

    printf("\nHEXADECIMAL LITERALS:\n");
    for (int i = 0; i < lit_count; i++)
    {
        if (CONSTTAB[i].type == 'X')
            printf("%s\n", CONSTTAB[i].constant);
    }

    printf("\nOCTAL LITERALS:\n");
    for (int i = 0; i < lit_count; i++)
    {
        if (CONSTTAB[i].type == 'O')
            printf("%s\n", CONSTTAB[i].constant);
    }
}

void pass1()
{
    FILE* symtab = fopen("symtab.txt", "w");
    if (symtab == NULL) { perror("Error opening symtab.txt"); exit(1); }

    FILE* f = fopen("instructions.txt", "r");
    if (f == NULL) { perror("Error opening instructions.txt"); fclose(symtab); exit(1); }

    FILE* output = fopen("intermediate.txt", "w");
    if (output == NULL) { perror("Error opening intermediate.txt"); fclose(symtab); fclose(f); exit(1); }

    char line[50];
    while (fgets(line, sizeof(line), f))
    {
        char label[100] = "", opcode[100] = "", operand[100] = "", temp[100] = "";

        int numFields = sscanf(line, "%s %s %s", label, opcode, operand);

        if (numFields <= 0) continue;

        if (numFields == 2) {
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
            if (strcmp(label, "_") != 0) {
                insert_into_symtab(symtab, label, pc);
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
                    continue;
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
                   CONSTTAB[lit_count].type = 'C';
                   strcpy(CONSTTAB[lit_count++].constant, operand);
                }
                else if (operand[0] == 'X')
                {
                    pc += (strlen(operand) - 3) / 2;
                    CONSTTAB[lit_count].type = 'X';
                    strcpy(CONSTTAB[lit_count++].constant, operand);
                }
                else if (operand[0] == 'O')
                {
                    pc += ceil(((strlen(operand) - 3) * 3) / 8.0);
                    CONSTTAB[lit_count].type = 'O';
                    strcpy(CONSTTAB[lit_count++].constant, operand);
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
    len = pc - start_addr;
    printf("Program length = %X\n", len);
    display_literals();
}

void pass2()
{
    struct {
        int location;
        char label[20];
        char operand[20];
        char opcode[20];
    } inter[MAX_INSTRUCTIONS]; //NO NEED OF NEW STRUCTURE, THE OLD CAN BE REUSED

    struct
{
    char symbols[20];
    int addr;
} SYMTAB[20];

    FILE* objp = fopen("obj_program.txt", "w");
    if (objp == NULL) {
        printf("Error in opening the object program file\n");
        exit(1);
    }

    FILE* interfile = fopen("intermediate.txt", "r");
    if (interfile == NULL) {
        printf("Error in opening the intermediate file\n");
        fclose(objp);
        exit(1);
    }

    FILE* symbolfile = fopen("symtab.txt", "r");
    if (symbolfile == NULL) {
        printf("Error in opening the saved SYMTAB\n");
        fclose(objp);
        exit(1);
    }


    int read_count = 0;
    int sym_count = 0;
    int start_address = 0;

    // Read instructions from the intermediate file
    while (fscanf(interfile, "%x %s %s %s\n",
                  &inter[read_count].location,
                  inter[read_count].label,
                  inter[read_count].opcode,
                  inter[read_count].operand) == 4) {
        read_count++;
        if (read_count >= MAX_INSTRUCTIONS) {
            break; // Prevent buffer overflow
        }
    }

    while (fscanf(symbolfile, "%s %x\n",
                  SYMTAB[sym_count].symbols,
                  &SYMTAB[sym_count].addr) == 2){
                  sym_count++;
                  }

    // Find the start address
    start_address = inter[0].location;

    // Prepare the name for the header
    char name[7] = {0}; // Ensure null-terminated string
    if (strlen(inter[0].label) < 6) {
        strcpy(name, inter[0].label);
        for (int j = strlen(inter[0].label); j < 6; j++) {
            name[j] = ' ';
        }
    } else {
        strncpy(name, inter[0].label, 6); // Copy first 6 characters
    }

    // Write header record
    fprintf(objp, "H^%s^%06X^%06X\n", name, start_address, len);

    // Text record initialization
    //printf("text record initialization\n");
    char text_record[70] = "";
    //printf("%s\n",text_record);
    char temp[10];
    int text_record_start = inter[0].location;
    int text_record_length = 0;
    int line_count = 0;

    for (int i = 0; i < read_count; i++) {
        int instr_length = 0;
        unsigned char opcode;
        int operand_address = 0;
        int indexed = 0;
        char obj_code[10] = "";

        // Search for opcode in OPTAB
        if (search_optab(inter[i].opcode, &instr_length)) {
            // Get opcode from OPTAB
            for (int j = 0; j < 30; j++) {
                if (!strcmp(inter[i].opcode, OPTAB[j].mnemonic)) {
                    opcode = OPTAB[j].opcode;
                    break;
                }
            }

            // Check for indexed addressing (indicated by operand,X)
            char *comma = strchr(inter[i].operand, ',');
            if (comma != NULL && !strcmp(comma + 1, "X")) {
                indexed = 1;
                *comma = '\0'; // Remove ',X' for symbol lookup
            }

            // Search SYMTAB for operand address
            int symbol_found = 0;
            for (int k = 0; k < sym_count; k++) {
                    //printf("%s\t%x\n",SYMTAB[k].symbols,SYMTAB[k].addr);
                if (!strcmp(SYMTAB[k].symbols, inter[i].operand)) {
                    operand_address = SYMTAB[k].addr;
                    symbol_found = 1;
                    break;
                }
            }

            // Check if symbol was found in SYMTAB, else flag error
            if (!symbol_found) {
                printf("Error: Undefined symbol '%s' at address %X\n", inter[i].operand, inter[i].location);
                continue;
            }

            // Add indexing bit (high-order bit in 12-bit address field)
            if (indexed) {
                operand_address |= 0x8000;
            }

            // Construct object code
            snprintf(obj_code, sizeof(obj_code), "%02X%04X", opcode, operand_address);
        } else if (!strcmp(inter[i].opcode, "WORD")) {
            // Handle WORD directive
            snprintf(obj_code, sizeof(obj_code), "%06X", atoi(inter[i].operand));
        } else if (!strcmp(inter[i].opcode, "BYTE")) {
            // Handle BYTE directive
            if (inter[i].operand[0] == 'C') {
                for (int j = 2; inter[i].operand[j] != '\'' && inter[i].operand[j] != '\0'; j++) {
                    snprintf(temp, sizeof(temp), "%02X", inter[i].operand[j]);
                    strcat(obj_code, temp);
                }
            } else if (inter[i].operand[0] == 'X') {
                strncpy(obj_code, inter[i].operand + 2, strlen(inter[i].operand) - 3);
            }
        } else if (!strcmp(inter[i].opcode, "RESW") || !strcmp(inter[i].opcode, "RESB")) {
            // Handle RESW and RESB (reset text record)
            if (text_record_length > 0) {
                fprintf(objp, "T^%06X^%02X^%s\n", text_record_start, text_record_length, text_record);
                strcpy(text_record, "");
                text_record_start = inter[i].location + instr_length; //Is this required??
                text_record_length = 0;

            }
            text_record_start = inter[i].location;
            continue;
        }
        else
        {
            if (strlen(text_record) > 0)
                printf("INVALID OPCODE\n");
        }

        // Append object code to text record if space allows
        if (text_record_length + strlen(obj_code) / 2 > 30) {
            // Write current text record
            fprintf(objp, "T^%06X^%02X^%s\n", text_record_start, text_record_length, text_record);

            strcpy(text_record, "");
            text_record_start = inter[i].location;
            text_record_length = 0;
            continue;
        }

        // Add object code to text record
        if (strlen(text_record) > 0) {
        strcat(text_record, "^");
        }
        strcat(text_record, obj_code);
        text_record_length += strlen(obj_code) / 2;
    }

    // Write final text record
    if (text_record_length > 0) {
        fprintf(objp, "%06X^%02X^%s\n", text_record_start, text_record_length, text_record);
    }

    // Write end record
    fprintf(objp, "E^%06X\n", start_address);

    // Close files
    fclose(interfile);
    fclose(objp);
}



int main()
{
    pass1();
    pass2();
    return 0;
}
