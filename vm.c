
#include <stdio.h>

struct Register{
    int OP;
    int L;
    int M;
};

int PAS[500] = {0}; // top = text segment (code), below text segment = stack segment (data)

// Prints Op code, level, mField, pc, bp index, sp index
// Then prints the contents of the stack from the top of the stack down to the base pointer
// Seperates activation records with a vertical bar
void printInfo(int pc, int *bp, int *sp, int op, int m, int l, int start){
    if(op == 1){ // LIT
        printf("LIT");
    } else if(op == 2){ // OPR
        if(m == 0){ // RTN
            printf("RTN");
        } else if (m == 1){ // ADD
            printf("ADD");
        } else if (m == 2){ // SUB
            printf("SUB");
        } else if (m == 3){ // MUL
            printf("MUL");
        } else if (m == 4){ // DIV
            printf("DIV");
        } else if (m == 5){ // EQL
            printf("EQL");
        } else if (m == 6){ // NEQ
            printf("NEQ");
        } else if (m == 7){ // LSS
            printf("LSS");
        } else if (m == 8){ // LEQ
            printf("LEQ");
        } else if (m == 9){ // GTR
            printf("GTR");
        } else if (m == 10){ // GEQ
            printf("GEQ");
        } else if(m == 11){
            printf("EVEN");
        }
    } else if(op == 3){ // LOD
        printf("LOD");
    } else if(op == 4){ // STO
        printf("STO");
    } else if(op == 5){ // CAL
        printf("CAL");
    } else if(op == 6){ // INC
        printf("INC");
    } else if(op == 7){ // JMP
        printf("JMP");
    } else if(op == 8){ // JPC
        printf("JPC");
    } else if(op == 9){ // SYS
        printf("SYS");
    }

    printf("\t%d \t%d \t%d \t%ld \t%ld", l, m, pc, 499 - (&PAS[499] - bp), 499 - (&PAS[499] - sp)); // Before stack
    printf("\t"); // After stack
    for(int i = start; i >= 499 - (&PAS[499] - sp); i--){ // How can i decide when a new AR begins, 499 - (&PAS[499] - bp)
        if(i == 499 - (&PAS[499] - bp) && i != start)
            printf("\t|\t");
        printf("%d ", PAS[i]);
    }
    printf("\n");
}

/* Find base L levels down from the current activation record */
int base (int BP , int L) {
    int arb = BP ; // activation record base 

    while (L > 0) {
        arb = PAS[arb]; // follow static link
        L--;
    }

    return arb ;
}

// Instruction set includes LIT, OPR, LOD, STO, CAL, INC, JMP, JPC, SYS
// Only compiles with one input file
// Prints output to console not another file
// Only 3 function (1 main, 2 helper)
int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Argument Count Incorrect");
        return 1;
    }


    FILE *file = fopen(argv[1], "r");
    if(!file){
        printf("ERROR");
        return 1;
    }

    int opcode = 0; 
    int level = 0; // Lexicographical level 
    int mField = 0; 
    int PC = 499; // 3 index's per instruction, PC -= 3;

    // Loops through input file to configure PAS
    while(fscanf(file, "%d %d %d", &opcode, &level, &mField) == 3){
        PAS[PC] = opcode;
        PAS[PC-1] = level;
        PAS[PC-2] = mField;

        PC -= 3;
    };
    fclose(file);

    int *SP = &PAS[PC+1];
    int *BP = &PAS[PC];
    int stackStart = 499 - (&PAS[499] - BP);
    PC = 499; 
    struct Register IR;

    // Initial Values;
    printf("        L       M       PC      BP      SP      stack\n");
    printf("Initial values: \t%d \t%ld \t%ld\n", PC, 499 - (&PAS[499] - BP), 499 - (&PAS[499] - SP)); // last arguments calculate the index of their respective pointer
    while (1){ // Loop till PAS reaches SYS 0 3 instruction, then exit
        IR.OP = PAS[PC];
        IR.L = PAS[PC-1];
        IR.M = PAS[PC-2];
        PC -= 3;

        // Core instrucion set
        if(IR.OP == 1){ // LIT
            SP--;
            PAS[499 - (&PAS[499] - SP)] = IR.M;
            printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
        } else if(IR.OP == 2){ // OPR
            if(IR.M == 0){ // RTN
                SP = BP + 1;
                BP = &PAS[PAS[(499 - (&PAS[499] - SP)) - 2]];
                PC = PAS[(499 - (&PAS[499] - SP)) - 3];
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 1){ // ADD
                PAS[(499 - (&PAS[499] - SP)) + 1] = PAS[(499 - (&PAS[499] - SP)) + 1] + PAS[499 - (&PAS[499] - SP)];
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 2){ // SUB
                PAS[(499 - (&PAS[499] - SP)) + 1] = PAS[(499 - (&PAS[499] - SP)) + 1] - PAS[499 - (&PAS[499] - SP)];
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 3){ // MUL
                PAS[(499 - (&PAS[499] - SP)) + 1] = PAS[(499 - (&PAS[499] - SP)) + 1] * PAS[499 - (&PAS[499] - SP)];
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 4){ // DIV
                PAS[(499 - (&PAS[499] - SP)) + 1] = PAS[(499 - (&PAS[499] - SP)) + 1] / PAS[499 - (&PAS[499] - SP)];
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 5){ // EQL
                PAS[(499 - (&PAS[499] - SP)) + 1] = (PAS[(499 - (&PAS[499] - SP)) + 1] == PAS[499 - (&PAS[499] - SP)]);
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 6){ // NEQ
                PAS[(499 - (&PAS[499] - SP)) + 1] = (PAS[(499 - (&PAS[499] - SP)) + 1] != PAS[499 - (&PAS[499] - SP)]);
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 7){ // LSS
                PAS[(499 - (&PAS[499] - SP)) + 1] = (PAS[(499 - (&PAS[499] - SP)) + 1] < PAS[499 - (&PAS[499] - SP)]);
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 8){ // LEQ
                PAS[(499 - (&PAS[499] - SP)) + 1] = (PAS[(499 - (&PAS[499] - SP)) + 1] <= PAS[499 - (&PAS[499] - SP)]);
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 9){ // GTR
                PAS[(499 - (&PAS[499] - SP)) + 1] = (PAS[(499 - (&PAS[499] - SP)) + 1] > PAS[499 - (&PAS[499] - SP)]);
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if (IR.M == 10){ // GEQ
                PAS[(499 - (&PAS[499] - SP)) + 1] = (PAS[(499 - (&PAS[499] - SP)) + 1] >= PAS[499 - (&PAS[499] - SP)]);
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if(IR.M == 11){ // EVEN
                PAS[499 - (&PAS[499] - SP)] = (PAS[499 - (&PAS[499] - SP)]) % 2 == 0;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else
                printf("Unknown OPR mField: %d\n", IR.M);
        } else if(IR.OP == 3){ // LOD
            SP--;
            PAS[499 - (&PAS[499] - SP)] = PAS[base(499 - (&PAS[499] - BP), IR.L) - IR.M];
            printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
        } else if(IR.OP == 4){ // STO
            PAS[base((499 - (&PAS[499] - BP)), IR.L) - IR.M] = PAS[499 - (&PAS[499] - SP)];
            SP++;
            printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
        } else if(IR.OP == 5){ // CAL
            PAS[(499 - (&PAS[499] - SP)) - 1] = base(499 - (&PAS[499] - BP), IR.L);
            PAS[(499 - (&PAS[499] - SP)) - 2] = 499 - (&PAS[499] - BP);
            PAS[(499 - (&PAS[499] - SP)) - 3] = PC;
            BP = SP - 1;
            PC = 499 - IR.M;
            printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
        } else if(IR.OP == 6){ // INC
            SP-= IR.M;
            printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
        } else if(IR.OP == 7){ // JMP
            PC = 499 - IR.M;
            printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
        } else if(IR.OP == 8){ // JPC
            if(PAS[499 - (&PAS[499] - SP)] == 0){
                PC = 499 - IR.M;
                SP++;
            }    
            printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
        } else if(IR.OP == 9){ // SYS
            if(IR.M == 1){ // PRINT
                printf("Output result is: %d\n", PAS[499 - (&PAS[499] - SP)]);
                SP++;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if(IR.M == 2){ // READ
                int input = 0;
                printf("Please Enter an Integer: ");
                scanf("%d", &input);
                SP--;
                PAS[499 - (&PAS[499] - SP)] = input;
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
            } else if(IR.M == 3){ // HALT
                printInfo(PC, BP, SP, IR.OP, IR.M, IR.L, stackStart);
                break;
            } else
                printf("Unknown SYS mField: %d\n", IR.M);
        } else
            printf("Unknown Op Code: %d\n", IR.OP);
    }
    
    return 0;
}