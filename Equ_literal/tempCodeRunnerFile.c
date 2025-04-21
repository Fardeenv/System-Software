        {
            unsigned long equ_value = 0;
            char expression[100];
            strcpy(expression, operand);

            // Evaluate the EQU expression
            char *token = strtok(expression, "+-*/");
            int result = 0;
            char operator = '+';

            while (token)
            {
                int value = 0;

                if (isdigit(token[0]))
                {
                    // Numeric constant
                    value = atoi(token);
                }
                else
                {
                    // Symbolic constant
                    int found = 0;
                    for (int k = 0; k < sym_count; k++)
                    {
                        if (!strcmp(SYMTAB[k].symbols, token))
                        {
                            value = SYMTAB[k].addr;
                            found = 1;
                            break;
                        }
                    }
                    if (!found)
                    {
                        printf("Error: Undefined symbol '%s' in EQU expression\n", token);
                        break;
                    }
                }

                // Perform arithmetic based on the operator
                if (operator == '+')
                    result += value;
                else if (operator == '-')
                    result -= value;
                else if (operator == '*')
                    result *= value;
                else if (operator == '/')
                {
                    if (value == 0)
                    {
                        printf("Error: Division by zero in EQU expression\n");
                        break;
                    }
                    result /= value;
                }

                // Get the next operator and token
                char *next_operator = strpbrk(token + strlen(token) + 1, "+-*/");
                operator = next_operator ? *next_operator : '+';
                token = strtok(NULL, "+-*/");
            }

            equ_value = result;

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
