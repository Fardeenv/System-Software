#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define _OPTAB_ "optab.txt"
#define _INFILE_ "input_fi.txt"
#define _OUTFILE_ "out_gpt.txt"

int optcount = 0, symcount = 0;

struct optab {
    char opcode[10];
    int val;
} op[100];

typedef struct addre {
    int address;
    char code_line[200];
    struct addre *next;
} sym_addr;

struct symtab {
    char name[30];
    int val;
    int def; // 0 if not defined, 1 if defined
    sym_addr *start;
} sym[100];

// Function Prototypes
int searchop(char *o);
void sepcoma(char *str, char *r, char *p);
int makeob(int opcode, int ind, int oprend);
int vget(char *line, char *label, char *opc, char *opr);
int write_textrec(FILE *, int, int, int, int);
int search_sym(const char *name);
int is_sym_def(const char *name);
int get_sym_value(const char *name);
void add_sym_undef(char *symname, int address, const char *codeline);
void add_sym_def(char *symname, int value, FILE *fout);
void loadop();
int print_op();
int skipcomment(char *p);
int getbyte(char *p);
int rstart2(const char *filein);
int isnulls(char *p);
void one_pass(const char *input, const char *output);

// Searches for an opcode in the opcode table
int searchop(char *o) {
    for (int i = 0; i < optcount; i++) {
        if (strcmp(op[i].opcode, o) == 0) {
            return op[i].val;
        }
    }
    return -1; // Opcode not found
}

// Searches for a symbol in the symbol table
int search_sym(const char *name) {
    for (int i = 0; i <= symcount; i++) {
        if (strcmp(sym[i].name, name) == 0)
            return i;
    }
    return -2; // Symbol not found
}

// Checks if a symbol is defined
int is_sym_def(const char *name) {
    int k = search_sym(name);
    if (k == -2)
        return 0; // Symbol not found
    return sym[k].def;
}

// Gets the value of a symbol
int get_sym_value(const char *name) {
    int k = search_sym(name);
    if (k == -2)
        return -1; // Symbol not found
    return sym[k].val;
}

// Adds an undefined symbol to the symbol table
void add_sym_undef(char *symname, int address, const char *codeline) {
    int k = search_sym(symname);
    sym_addr *po, *t;
    if (k == -2) { // Symbol not found, create a new entry
        strcpy(sym[symcount].name, symname);
        sym[symcount].val = -1;
        sym[symcount].def = 0;
        po = (sym_addr *)malloc(sizeof(sym_addr));
        po->address = address;
        strcpy(po->code_line, codeline);
        po->next = NULL;
        sym[symcount].start = po;
        symcount++;
    } else { // Symbol found, add to its address list
        t = sym[k].start;
        while (t->next != NULL) {
            t = t->next;
        }
        po = (sym_addr *)malloc(sizeof(sym_addr));
        po->address = address;
        strcpy(po->code_line, codeline);
        po->next = NULL;
        t->next = po;
    }
}

// Adds a defined symbol to the symbol table
void add_sym_def(char *symname, int value, FILE *fout) {
    int k = search_sym(symname);
    sym_addr *po, *t;
    if (k == -2) { // Symbol not found, create a new entry
        strcpy(sym[symcount].name, symname);
        sym[symcount].val = value;
        sym[symcount].def = 1;
        sym[symcount].start = NULL;
        symcount++;
    } else { // Symbol found
        if (is_sym_def(symname)) { // Error: Duplicate definition
            printf("ERROR: Duplicate symbol definition: %s\n", symname);
        } else { // Resolve undefined references
            sym[k].def = 1;
            sym[k].val = value;
            po = sym[k].start;
            sym[k].start = NULL;
            if (po != NULL) {
                while (po != NULL) {
                    // Resolve undefined references here
                    fprintf(fout, "Resolved reference at address %06X for symbol %s\n", po->address, symname);
                    t = po;
                    po = po->next;
                    free(t);
                }
            }
        }
    }
}

// Loads the opcode table from a file
void loadop() {
    FILE *p = fopen(_OPTAB_, "r");
    int opv;
    char li[50], opc[10], opvc[10], opp[10];
    int i = 0;
    if (!p) {
        printf("\nERROR: Unable to open opcode file.\n");
        exit(1);
    }
    while (fgets(li, sizeof(li), p)) {
        vget(li, opc, opvc, opp);
        strcpy(op[i].opcode, opc);
        sscanf(opvc, "%x", &op[i].val);
        i++;
    }
    optcount = i;
    fclose(p);
}

// The main pass of the assembler
void one_pass(const char *input, const char *output) {
    FILE *fin = fopen(input, "r");
    FILE *fout = fopen(output, "w");
    int start = rstart2(input), end = 0;
    int count = start, temp = 0, wordb = 0, opcode = 0, operand = 0, ind = 0;
    char label[100], opc[100], opr[100], li[200], opr1[50], opr2[10];

    if (!fout || !fin) {
        printf("\nERROR: Unable to open input or output file.\n");
        exit(1);
    }

    while (fgets(li, sizeof(li), fin)) {
        if (!isnulls(li))
            continue;

        vget(li, label, opc, opr);
        sepcoma(opr, opr1, opr2);

        if (strcmp(label, "") != 0) {
            add_sym_def(label, count, fout);
        }

        if ((opcode = searchop(opc)) >= 0) {
            if (is_sym_def(opr1)) {
                operand = get_sym_value(opr1);
                ind = (opr2[0] == '\0') ? 0 : 1;
                wordb = makeob(opcode, ind, operand);
                write_textrec(fout, count, 3, wordb, 0);
            } else if (opr1[0] != '\0') {
                add_sym_undef(opr1, count, li);
                wordb = makeob(opcode, ind, 0);
                write_textrec(fout, count, 3, wordb, 0);
            } else {
                wordb = makeob(opcode, ind, 0);
                write_textrec(fout, count, 3, wordb, 0);
            }
            count += 3;
        } else if (strcmp(opc, "END") == 0) {
            end = count;
            fprintf(fout, "E^%06X\n", start);
            break;
        } else {
            printf("ERROR: Invalid opcode: %s\n", opc);
            break;
        }
    }

    for (int i = 0; i < symcount; i++) {
        if (sym[i].def == 0) {
            printf("ERROR: Undefined symbol: %s\n", sym[i].name);
        }
    }

    fclose(fin);
    fclose(fout);
}

// Main function
int main() {
    loadop();
    one_pass(_INFILE_, _OUTFILE_);
    return 0;
}
