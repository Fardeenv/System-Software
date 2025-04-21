#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 20

struct estab {
    char csname[10];
    char extsym[10];
    int address;
    int length;
} es[MAX];

int searchEstab(char *symbol, int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(es[i].extsym, symbol) == 0 || strcmp(es[i].csname, symbol) == 0) {
            return 1; 
        }
    }
    return 0; 
}


void main() {
    char input[10], name[10], symbol[10];
    int count = 0;
    unsigned long progaddr, csaddr, add, len;
    FILE *fp1, *fp2;

    fp1 = fopen("linkinput2.txt", "r");
    if (!fp1) {
        printf("Error: Could not open input file.\n");
        exit(1);
    }

    fp2 = fopen("ESTAB.txt", "w");
    if (!fp2) {
        printf("Error: Could not create output file.\n");
        fclose(fp1);
        exit(1);
    }

    printf("Enter the location where the program has to be loaded: ");
    scanf("%lX", &progaddr);
    csaddr = progaddr;

    fscanf(fp1, "%s", input);

    while (strcmp(input, "END") != 0) 
    {
        if (strcmp(input, "H") == 0) 
        {
            fscanf(fp1, "%s", name);

            if (searchEstab(name, count)) 
            {
                printf("Error: Duplicate control section name '%s' found.\n", name);
                strcpy(es[count].csname, name);
                strcpy(es[count].extsym, "**");
                fscanf(fp1, "%lX", &add);
                es[count].address = add + csaddr;
                fscanf(fp1, "%lX", &len);
                es[count].length = len;
                count++;
                fscanf(fp1, "%s", input);
                continue;
            }

            strcpy(es[count].csname, name);
            strcpy(es[count].extsym, "**");
            fscanf(fp1, "%lX", &add);
            es[count].address = add + csaddr;
            fscanf(fp1, "%lX", &len);
            es[count].length = len;
            fprintf(fp2, "%s %s %lX %lX\n", es[count].csname, es[count].extsym, es[count].address, es[count].length);
            count++;
        } 
        else if (strcmp(input, "D") == 0) 
        {
            fscanf(fp1, "%s", input);
            while (strcmp(input, "R") != 0) 
            {
                if (searchEstab(input, count)) 
                {
                    printf("Error: Duplicate external symbol '%s' found.\n", input);
                    strcpy(es[count].csname, "**");
                    strcpy(es[count].extsym, input);
                    fscanf(fp1, "%lX", &add);
                    es[count].address = add + csaddr;
                    es[count].length = 0;
                    count++;
                    fscanf(fp1, "%s", input);
                    continue;
                }

                strcpy(es[count].csname, "**");
                strcpy(es[count].extsym, input);
                fscanf(fp1, "%lX", &add);
                es[count].address = add + csaddr;
                es[count].length = 0;
                fprintf(fp2, "%s %s %lX %lX\n", es[count].csname, es[count].extsym, es[count].address, es[count].length);
                count++;
                fscanf(fp1, "%s", input);
            }
            csaddr = csaddr + len;
        } 
        else if (strcmp(input, "T") == 0) 
        {
            while (strcmp(input, "E") != 0)
                fscanf(fp1, "%s", input);
        }
        fscanf(fp1, "%s", input);
    }

    fclose(fp1);
    fclose(fp2);
    printf("Linking loader pass 1 completed. Output written to 'ESTAB.txt'.\n");
}
