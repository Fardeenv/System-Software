
#include <stdio.h>
#include <string.h>

char input[10], label[10], ch1, ch2;
int addr, w = 0, start, ptaddr, l, length = 0, end, count = 0, k, taddr, address, i = 0;
FILE *fp1, *fp2;

// Packing function
unsigned char pack_hex(char high, char low) {
    unsigned char h = high - '0';
    if (h > 9) h -= 7; // Adjust for 'A-F'
    unsigned char l = low - '0';
    if (l > 9) l -= 7; // Adjust for 'A-F'
    return (h << 4) | l; // Combine high and low nibbles
}

void check();
void process_record(const char *record);
void main() {
    fp1 = fopen("input.txt", "r");
    fp2 = fopen("output.txt", "w");

    fscanf(fp1, "%s", input);
    printf("\n\nABSOLUTE LOADER\n");
    fprintf(fp2, "\n-------------------------------------------------------\n");
    fprintf(fp2, "MEMORY ADDRESS\t\t\tCONTENTS");
    fprintf(fp2, "\n-------------------------------------------------------\n");

    while (strcmp(input, "E") != 0) {
        if (strcmp(input, "H") == 0) {
            fscanf(fp1, "%s %x %x %s", label, &start, &end, input);
            address = start;
        } else if (strcmp(input, "T") == 0) {
            l = length;
            ptaddr = addr;
            fscanf(fp1, "%x %x", &taddr, &length);

            addr = taddr;
            if (w == 0) {
                ptaddr = address;
                w = 1;
            }

            // Fill memory gaps with "xx"
            for (k = 0; k < (taddr - (ptaddr + l)); k++) {
                address = address + 1;
                fprintf(fp2, "xx");
                count++;
                if (count == 4) {
                    fprintf(fp2, "  ");
                    i++;
                    if (i == 4) {
                        fprintf(fp2, "\n\n%x\t\t", address);
                        i = 0;
                    }
                    count = 0;
                }
            }
            if (taddr == start) {
                fprintf(fp2, "\n\n%x\t\t", taddr);
            }

            // Process the text record object code
            fscanf(fp1, "%s", input);
            process_record(input);

        } else {
            // Process data from input file
            process_record(input);
            fscanf(fp1, "%s", input);
        }
    }

    fprintf(fp2, "\n-------------------------------------------------------\n");
    fclose(fp1);
    fclose(fp2);

    printf("\n\nThe contents of the output file:\n");
    fp2 = fopen("output.txt", "r");
    ch2 = fgetc(fp2);
    while (ch2 != EOF) {
        printf("%c", ch2);
        ch2 = fgetc(fp2);
    }
    fclose(fp2);
}

void process_record(const char *record) {
    int idx = 0;
    while (record[idx] != '\0')
        {
        // Pack and write hex
        unsigned char byte = pack_hex(record[idx], record[idx + 1]);
        fprintf(fp2, "%02x", byte);

        check();
        idx += 2; // Move to next pair of characters
    }
}

// Keeps track of formatting (4 groups of 4 bytes per line)
void check() {
    count++;
    address++;
    taddr = taddr + 1;
    if (count == 4) {
        fprintf(fp2, "  ");
        i++;
        if (i == 4) {
            fprintf(fp2, "\n\n%x\t\t", taddr);
            i = 0;
        }
        count = 0;
    }
}
