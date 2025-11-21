
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_TOKEN_SIZE 500
#define MAX_INSTRUCTION_SIZE 500

// File names
const char inputFileName[] = "token_list.txt";
const char outputFileName[] = "elf.txt";

typedef struct{
    int op;
    int l;
    int m;
} Instruction;

Instruction instructions[MAX_INSTRUCTION_SIZE];
int instIndex = 0;

// The token value is 2 off from the scanners token value so before you use it tokenInputValue - 2, unless tokenInputValue == 1 then just print lexical error and exit().
typedef enum {
    identsym = 0, // Identifier
    numbersym, // Number
    plussym, // +
    minussym, // -
    multsym, // *
    slashsym, // /
    eqsym, // =
    neqsym, // <>
    lessym, // <
    leqsym, // <=
    gtrsym, // >
    geqsym, // >=
    lparentsym, // (
    rparentsym, // )
    commasym, // ,
    semicolonsym, // ;
    periodsym, // .
    becomessym, // :=
    beginsym, // begin
    endsym, // end
    ifsym, // if
    fisym, // fi
    thensym, // then
    whilesym, // while
    dosym, // do
    callsym, // call 
    constsym, // const
    varsym, // var
    procsym, // procedure 
    writesym, // write
    readsym, // read
    elsesym, // else
    evensym, // even
} TokenType;

typedef struct{
    char name[12]; // For Id's
    int value; // For Num's
    TokenType type;
} Token;

// Global Vars
Token tokens[MAX_TOKEN_SIZE]; // Holds all tokens from input file so no need to keep file open
int tokenCount = 0;
int currentToken = 0; // Index for which token parser is currently at
FILE *out;

int symbolTableInsert(int kind, char s[], int val, int lexLevel, int addr);
int symbolTableCheck(char s[]);
void symbolTableMark(int i);
void error(char *message);
void emit(int op, int l, int m);

typedef struct {
    int kind; // const = 1, var = 2, procedure = 3
    char name[12]; // name up to 11 chars
    int val; // const values
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted, 0 = use, 1 = cant use
} symbol;

symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
int symCount = 0;

// Adds a new symbol to table. Checks if dup exist, if so return -1, else return 1.
int symbolTableInsert(int kind, char s[], int val, int lexLevel, int addr){
    
    if (symbolTableCheck(s) != -1)
        error("Error: symbol name has already been declared");

    if (symCount >= MAX_SYMBOL_TABLE_SIZE)
        error("Symbol Table Overflow Error");

    symbolTable[symCount].kind = kind;
    strcpy(symbolTable[symCount].name, s);
    symbolTable[symCount].val = val;
    symbolTable[symCount].level = lexLevel;
    symbolTable[symCount].addr = addr;
    symbolTable[symCount].mark = 0;
    symCount++;
}

// linear search through symbol table looking at name return index if found, -1 if not
int symbolTableCheck(char s[]){
    for(int i = symCount - 1; i >= 0; i--){ // Start from newest (localest) for shadowing variables
        if (strcmp(symbolTable[i].name, s) == 0 && symbolTable[i].mark == 0)
            return i;
    }
    return -1;
}

// Set the mark field to 1 to indicate the symbol can be no longer used
void symbolTableMark(int i){
    if(i >= 0 && i < symCount)
        symbolTable[i].mark = 1;
}

void emit(int op, int l, int m){
    if(instIndex >= MAX_INSTRUCTION_SIZE)
        error("Instructions Overflow Error");

    instructions[instIndex].op = op;
    instructions[instIndex].l = l;
    instructions[instIndex].m = m;
    instIndex++;
}

// Increments token index by 1
void getNextToken(){
    if(currentToken < tokenCount - 1)
        currentToken++;
}

// Grabs and returns current token type
Token peek(){
    return tokens[currentToken];
}

void error(char *message){
    printf("%s", message);
    fprintf(out, "%s", message);
    fclose(out);
    exit(0);
}

void program();
void block(int lexLevel);
void constDeclaration(int lexLevel);
int varDeclaration(int lexLevel);
void procDeclaration(int lexLevel);
void statement(int lexLevel);
void condition();
void expression();
void term();
void factor();

void program(){
    block(0);
    if(peek().type != periodsym)
        error("Error: program must end with period");
    
    emit(9, 0, 3);
}

void block(int lexLevel){
    int memAlloc = 3;

    constDeclaration(lexLevel);
    int numVars = varDeclaration(lexLevel);
    int jmpIdx = instIndex;
    emit(7, 0, 0);
    procDeclaration(lexLevel);
    instructions[jmpIdx].m = instIndex * 3;
    emit(6, 0, memAlloc + numVars);
    statement(lexLevel);

    // Cleans up all variables local to that block/procedure
    for(int i = symCount - 1; i >= 0; i--){
        if(symbolTable[i].level == lexLevel)
            symbolTable[i].mark = 1;
        else if(symbolTable[i].level < lexLevel)
            break;
    }
}

void constDeclaration(int lexLevel){
    if(peek().type == constsym){ 
        do{
            getNextToken();
            if (peek().type != identsym) 
                error("Error: const, var, read, procedure, and call keywords must be followed by identifier");
            int symIdx = symbolTableCheck(peek().name);
            if(symIdx != -1 && symbolTable[symIdx].level == lexLevel && symbolTable[symIdx].mark == 0) // Mark should always be true if the other two were true but its a just in case
                error("Error: symbol name has already been declared");
            char name[12];
            strcpy(name, peek().name);
            getNextToken();
            if (peek().type != eqsym) 
                error("Error: constants must be assigned with =");
            getNextToken();
            if (peek().type != numbersym) 
                error("Error: constants must be assigned an integer value");
            symbolTableInsert(1, name, peek().value, lexLevel, 0);
            getNextToken();
        } while (peek().type == commasym);
        if (peek().type != semicolonsym) 
            error("Error: constant and variable declarations must be followed by a semicolon");
        getNextToken();
    }
}

int varDeclaration(int lexLevel) {
    int numVars = 0;
    if(peek().type == varsym){
        do{
            getNextToken();
            if(peek().type != identsym) 
                error("Error: const, var, read, procedure, and call keywords must be followed by identifier");
            int symIdx = symbolTableCheck(peek().name);
            if(symIdx != -1 && symbolTable[symIdx].level == lexLevel && symbolTable[symIdx].mark == 0) // Mark should always be true if the other two were true but its a just in case
                error("Error: symbol name has already been declared");
            symbolTableInsert(2, peek().name, 0, lexLevel, 3 + numVars); // 3 is the memAlloc from block reserved for the RA/DL/Sl
            numVars++;
            getNextToken();
        } while(peek().type == commasym);
        if(peek().type != semicolonsym) 
            error("Error: constant and variable declarations must be followed by a semicolon");
        getNextToken();
    }

    return numVars;
}

void procDeclaration(int lexLevel){
    while(peek().type == procsym){
        getNextToken();
        if(peek().type != identsym) 
            error("Error: const, var, read, procedure, and call keywords must be followed by identifier");
        symbolTableInsert(3, peek().name, 0, lexLevel, instIndex * 3);
        getNextToken();
        if(peek().type != semicolonsym) 
            error("Error: procedure declaration must be followed by a semicolon"); // Checks for semicolon that ends procedure name
        getNextToken();
        block(lexLevel + 1);
        emit(2, 0, 0);
        if(peek().type != semicolonsym)  // Checks for semicolon that ends procedure definition so we can move onto next procedure declaration or driver procedure
            error("Error: procedure declaration must be followed by a semicolon");
        getNextToken();
    }
}

void statement(int lexLevel){
    if(peek().type == identsym){
        int symIdx = symbolTableCheck(peek().name);
        if(symIdx == -1)
            error("Error: undeclared identifier");
        if(symbolTable[symIdx].kind != 2)
            error("Error: only variable values may be altered");
        getNextToken();
        if(peek().type != becomessym) 
            error("Error: assignment statements must use :=");
        getNextToken();
        expression();
        emit(4, 0, symbolTable[symIdx].addr);
    }
    else if(peek().type == callsym){
        getNextToken();
        if(peek().type != identsym) 
            error("Error: const, var, read, procedure, and call keywords must be followed by identifier");
        int symIdx = symbolTableCheck(peek().name);
        if(symIdx == -1)
            error("Error: undeclared identifier");
        if(symbolTable[symIdx].kind != 3)
            error("Error: call statement may only target procedures"); 
        emit(5, lexLevel - symbolTable[symIdx].level, symbolTable[symIdx].addr);
        getNextToken();
    }
    else if(peek().type == beginsym){
        do{
            getNextToken();
            statement(lexLevel);
        } while (peek().type == semicolonsym);
        if(peek().type != endsym) 
            error("Error: begin must be followed by end");
        getNextToken();
    }
    else if(peek().type == ifsym){
        getNextToken();
        condition();
        if(peek().type != thensym) 
            error("Error: if must be followed by then");
        getNextToken();    
        int jpcIdx = instIndex; // Jump to else if condition is false (JPC reads 0 from condition set up)
        emit(8, 0, 0);    
        statement(lexLevel);
        int jmpIdx = instIndex; // Dont worry about else, condition is true so skip to the end
        emit(7,0,0);
        instructions[jpcIdx].m = instIndex * 3;
        if(peek().type != elsesym)
            error("Error: if statement must include else clause");
        getNextToken();
        statement(lexLevel);
        instructions[jmpIdx].m = instIndex * 3; // Each instIndex is a struct holding 3 "commads"/"parameters" therefore actual address is 3 * instIndex
        if(peek().type != fisym)
            error("Error: else must be followed by fi");
        getNextToken();
    }
    else if(peek().type == whilesym){
        getNextToken();
        int loopIdx = instIndex;
        condition();
        if(peek().type != dosym) 
            error("Error: while must be followed by do");
        getNextToken();
        int jpcIdx = instIndex;
        emit(8,0,0);
        statement(lexLevel);
        emit(7, 0, loopIdx);
        instructions[jpcIdx].m = instIndex * 3; // Each instIndex is a struct holding 3 "commads"/"parameters" therefore actual address is 3 * instIndex
    }
    else if(peek().type == readsym){
        getNextToken();
        if(peek().type != identsym) 
            error("Error: const, var, read, procedure, and call keywords must be followed by identifier");
        int symIdx = symbolTableCheck(peek().name);
        if(symIdx == -1)
            error("Error: undeclared identifier");
        if(symbolTable[symIdx].kind != 2)
            error("Error: only variable values may be altered");
        getNextToken();
        emit(9, 0, 2);
        emit(4, 0, symbolTable[symIdx].addr);
    }
    else if(peek().type == writesym){
        getNextToken();
        expression();
        emit(9, 0, 1);
    }
}

void condition(){
    if(peek().type == evensym){
        getNextToken();
        expression();
        emit(2, 0, 11);
    } 
    else{
        expression();
        if(peek().type == eqsym){
            getNextToken();
            expression();
            emit(2, 0, 5);
        }
        else if(peek().type == neqsym){
            getNextToken();
            expression();
            emit(2, 0, 6);
        }
        else if(peek().type == lessym){
            getNextToken();
            expression();
            emit(2, 0, 7);
        }
        else if(peek().type == leqsym){
            getNextToken();
            expression();
            emit(2, 0, 8);
        }
        else if(peek().type == gtrsym){
            getNextToken();
            expression();
            emit(2, 0, 9);
        }
        else if(peek().type == geqsym){
            getNextToken();
            expression();
            emit(2, 0, 10);
        }
        else
            error("Error: condition must contain comparison operator");
    }
}

void expression() {
    if(peek().type == plussym)
        getNextToken();
    term();
    while(peek().type == plussym || peek().type == minussym){
        if(peek().type == plussym){
            getNextToken();
            term();
            emit(2, 0, 1);
        }
        else{
            getNextToken();
            term();
            emit(2, 0, 2);
        }
    }
}


void term(){
    factor();
    while(peek().type == multsym || peek().type == slashsym){ 
        if(peek().type == multsym){
            getNextToken();
            factor();
            emit(2, 0, 3);
        }
        else if(peek().type == slashsym){
            getNextToken();
            factor();
            emit(2, 0, 4);
        }
    }
}

void factor(){
    if(peek().type == identsym){
        int symIdx = symbolTableCheck(peek().name);
        if(symIdx == -1)
            error("Error: undeclared identifier");
        if (symbolTable[symIdx].kind == 1)
            emit (1, 0, symbolTable[symIdx].val);
        else if(symbolTable[symIdx].kind == 2)
            emit(3, 0, symbolTable[symIdx].addr);
        getNextToken();
    }
    else if(peek().type == numbersym){ 
        emit(1, 0, peek().value);
        getNextToken();
    }
    else if(peek().type == lparentsym){
        getNextToken();
        expression();
        if (peek().type != rparentsym) 
            error("Error: right parenthesis must follow left parenthesis");
        getNextToken();
    } else
        error("Error: arithmetic equations must contain operands, parentheses, numbers, or symbols");
}

char *getOpName(int op, int m){
    switch (op) {
        case 1: return "LIT";
        case 2: return "OPR";
        case 3: return "LOD";
        case 4: return "STO";
        case 5: return "CAL";
        case 6: return "INC";
        case 7: return "JMP";
        case 8: return "JPC";
        case 9: return "SYS";
    }
}

void printToTerminal(){
    printf("\nAssembly Code:\n\n");
    printf("Line\t OP\tL\t  M\n");
    for(int i = 0; i < instIndex; i++){
        char *name = getOpName(instructions[i].op, instructions[i].m);
        printf("%4d\t%3s\t%1d\t%3d\n", i, name, instructions[i].l, instructions[i].m);
    }

    printf("\nSymbol Table:\n\n");
    printf("Kind | Name | Value | Level | Address | Mark\n");
    printf("----------------------------------------------\n");
    for(int i = 0; i < symCount; i++){
        printf("%4d | %4s | %5d | %5d | %7d | %4d\n", symbolTable[i].kind, symbolTable[i].name, symbolTable[i].val, symbolTable[i].level, symbolTable[i].addr, symbolTable[i].mark);
    }
}

void printToOutFile(){
    for(int i = 0; i < instIndex; i++)
        fprintf(out, "%d %d %d\n", instructions[i].op, instructions[i].l, instructions[i].m);
    
}

int main(int argc, int *argv[]){
    if(argc != 1){
        printf("argc ERROR");
        return 1;
    }

    out = fopen(outputFileName, "w");

    // Takes a single line of input and scans for int's as token type. Special cases: 1, 2, 3.
    FILE *in = fopen(inputFileName, "r");
    if(!in){
        printf("inFile Error");
        return 1;
    }

    while (fscanf(in, "%d", &tokens[tokenCount].type) == 1){
        if(tokens[tokenCount].type == 1){
            printf("Error: Scanning error detected by lexer (skipsym present)");
            fprintf(out, "Error: Scanning error detected by lexer (skipsym present)");
            fclose(out);
            fclose(in);
            exit(1);
        }
        if(tokens[tokenCount].type == 2) // Handles ID, enum value from scanner
            fscanf(in, "%s", tokens[tokenCount].name);
        else if(tokens[tokenCount].type == 3) // Handles Num, enum value from scanner
            fscanf(in, "%d", &tokens[tokenCount].value);
        
        tokenCount++;
    }
    fclose(in);

    for(int i = 0; i < tokenCount; i++)// Normalizes the tokens from enum of Lex.c to enum in this file
        tokens[i].type -= 2;

    program();

    printToTerminal();
    printToOutFile(); 
    fclose(out);
    return 0;
}
