/*
 ============================================================================
 Name        : Lc3b-Assembler.c
 Author      : 1sand0s
 Version     :
 Copyright   : Your copyright notice
 Description : An Assembler for the Lc3B ISA, Ansi-style
 ============================================================================
 */

/**
 * @author 1sand0s
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include "Lc3b-Assembler.h"

/** +
 * @def Debug_Print
 * @brief Comment to stop printing the tokens during lexing
 *
 */
//#define Debug_Print

/** +
 * @def Debug_Print_InstructionTable
 * @brief Comment to stop printing the Instruction table to stdout
 *
 */
#define Debug_Print_InstructionTable

/** +
 * @def Debug_Print_SymbolTable
 * @brief Comment to stop printing the Symbol table to stdout
 *
 */
#define Debug_Print_SymbolTable

/** +
 * @def INDENT_SYMBOL_
 * @brief Number of spaces for indenting symbol table during printing
 *
 */
#define INDENT_SYMBOL_ 40

/** +
 * @def INDENT_INSTRUCTION_
 * @brief Number of spaces for indenting instruction table during printing
 *
 */
#define INDENT_INSTRUCTION_ 10

/** +
 * @def MAX_INSTRUCTION_LENGTH_
 * @brief Maximum length/number of tokens allowed in a valid instruction
 *
 */
#define MAX_INSTRUCTION_LENGTH_ 5

/** +
 * @def MAX_VALID_COMBINATIONS_
 * @brief Number of valid combinations of tokens
 *
 */
#define MAX_VALID_COMBINATIONS_ 68


char * inval[43] = {
    "IN",
    "OUT",
    "GETC",
    "PUTS",
    ".ORIG",
    ".FILL",
    ".END",
    "ADD",
    "AND",
    "JMP",
    "JSR",
    "JSRR",
    "LDB",
    "LDW",
    "LEA",
    "NOP",
    "NOT",
    "RET",
    "LSHF",
    "RSHFL",
    "RSHFA",
    "RTI",
    "STB",
    "STW",
    "TRAP",
    "XOR",
    "HALT",
    "BR",
    "BRN",
    "BRZ",
    "BRP",
    "BRNZ",
    "BRNP",
    "BRZP",
    "BRNZP",
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6",
    "R7"
};

/* Valid State Transistions for tokens of an instruction */
enum pFSM pTransitions[MAX_VALID_COMBINATIONS_][MAX_INSTRUCTION_LENGTH_] = {
                                    {ORIG, IMM, IGN, IGN, IGN}      ,       {LABEL, ORIG, IMM, IGN, IGN},
                                    {FILL, IMM, IGN, IGN, IGN}      ,       {LABEL, FILL, IMM, IGN, IGN},
                                    {END, IGN, IGN, IGN, IGN}       ,       {LABEL, END, IGN, IGN, IGN},
                                    {ADD, REG, REG, IMM, IGN}       ,       {ADD, REG, REG, REG, IGN},
                                    {LABEL, ADD, REG, REG, IMM}     ,       {LABEL, ADD, REG, REG, REG},
                                    {AND, REG, REG, IMM, IGN}       ,       {AND, REG, REG, REG, IGN},
                                    {LABEL, AND, REG, REG, IMM}     ,       {LABEL, AND, REG, REG, REG},
                                    {XOR, REG, REG, IMM, IGN}       ,       {XOR, REG, REG, REG, IGN},
                                    {LABEL, XOR, REG, REG, IMM}     ,       {LABEL, XOR, REG, REG, REG},
                                    {BR, LABEL, IGN, IGN, IGN}      ,       {LABEL, BR, LABEL, IGN, IGN},
                                    {BRN, LABEL, IGN, IGN, IGN}     ,       {LABEL, BRN, LABEL, IGN, IGN},
                                    {BRZ, LABEL, IGN, IGN, IGN}     ,       {LABEL, BRZ, LABEL, IGN, IGN},
                                    {BRP, LABEL, IGN, IGN, IGN}     ,       {LABEL, BRP, LABEL, IGN, IGN},
                                    {BRNZ, LABEL, IGN, IGN, IGN}    ,       {LABEL, BRNZ, LABEL, IGN, IGN},
                                    {BRNP, LABEL, IGN, IGN, IGN}    ,       {LABEL, BRNP, LABEL, IGN, IGN},
                                    {BRZP, LABEL, IGN, IGN, IGN}    ,       {LABEL, BRZP, LABEL, IGN, IGN},
                                    {BRNZP, LABEL, IGN, IGN, IGN}   ,       {LABEL, BRNZP, LABEL, IGN, IGN},
                                    {JMP, REG, IGN, IGN, IGN}       ,       {LABEL, JMP, REG, IGN, IGN},
                                    {JSR, LABEL, IGN, IGN, IGN}     ,       {LABEL, JSR, LABEL, IGN, IGN},
                                    {JSRR, REG, IGN, IGN, IGN}      ,       {LABEL, JSRR, REG, IGN, IGN},
                                    {LDB, REG, REG, IMM, IGN}       ,       {LABEL, LDB, REG, REG, IMM},
                                    {LDW, REG, REG, IMM, IGN}       ,       {LABEL, LDW, REG, REG, IMM},
                                    {LEA, REG, LABEL, IGN, IGN}     ,       {LABEL, LEA, REG, LABEL, IGN},
                                    {NOT, REG, REG, IGN, IGN}       ,       {LABEL, NOT, REG, REG, IGN},
                                    {RET, IGN, IGN, IGN, IGN}       ,       {LABEL, RET, IGN, IGN, IGN},
                                    {RTI, IGN, IGN, IGN, IGN}       ,       {LABEL, RTI, IGN, IGN, IGN},
                                    {LSHF, REG, REG, IMM, IGN}      ,       {LABEL, LSHF, REG, REG, IMM},
                                    {RSHFL, REG, REG, IMM, IGN}     ,       {LABEL, RSHFL, REG, REG, IMM},
                                    {RSHFA, REG, REG, IMM, IGN}     ,       {LABEL, RSHFA, REG, REG, IMM},
                                    {STB, REG, REG, IMM, IGN}       ,       {LABEL, STB, REG, REG, IMM},
                                    {STW, REG, REG, IMM, IGN}       ,       {LABEL, STW, REG, REG, IMM},
                                    {TRAP, IMM, IGN, IGN, IGN}      ,       {LABEL, TRAP, IMM, IGN, IGN},
                                    {HALT, IGN, IGN, IGN, IGN}      ,       {LABEL, HALT, IGN, IGN, IGN},
                                    {NOP, IGN, IGN, IGN, IGN}       ,       {LABEL, NOP, IGN, IGN, IGN}
                                };

enum pFSM pStates[38] = {
    START,
    LABEL,
    ORIG,
    FILL,
    END,
    ADD,
    AND,
    JMP,
    JSR,
    JSRR,
    LDB,
    LDW,
    LEA,
    NOP,
    NOT,
    RET,
    LSHF,
    RSHFL,
    RSHFA,
    RTI,
    STB,
    STW,
    TRAP,
    XOR,
    HALT,
    BR,
    BRN,
    BRZ,
    BRP,
    BRNZ,
    BRNP,
    BRZP,
    BRNZP,
    INVALID,
    STOP,
    REG,
    IMM,
    IGN
};

/** +
 * @fn enum errorCode (*[])(instruction*, symbol*, int)
 * @brief Function pointers to encode instructions
 *
 * @param instruction*   Instruction to encode
 * @param symbol*        Symbol if any to replace with address
 * @param int
 * @return errorCode     Any errors during encoding process
 */
enum errorCode( * encoder[31])(instruction * , symbol * , int) = {
    encodeORIG,
    encodeFILL,
    encodeEND,
    encodeADD,
    encodeAND,
    encodeJMP,
    encodeJSR,
    encodeJSRR,
    encodeLDB,
    encodeLDW,
    encodeLEA,
    encodeNOP,
    encodeNOT,
    encodeRET,
    encodeLSHF,
    encodeRSHFL,
    encodeRSHFA,
    encodeRTI,
    encodeSTB,
    encodeSTW,
    encodeTRAP,
    encodeXOR,
    encodeHALT,
    encodeBR,
    encodeBRN,
    encodeBRZ,
    encodeBRP,
    encodeBRNZ,
    encodeBRNP,
    encodeBRZP,
    encodeBRNZP
};

/** +
 * @fn int main(int, char**)
 * @brief main method
 *
 * @param argc  Number of commnadline arguements
 * @param argv  Name of commandline arguements
 * @return int  Exit code
 */
int main(int argc, char ** argv) {

    enum errorCode errorp; //Holds the type error encountered during assembly

    /* Two arguements from command line expected
     * 1. argv[1] : Name of the file containing assembly code <*.asm>
     * 2. argv[2] : Name of the file to write assembled hex code to <*.hex> */
    if (argc == 3) {
        FILE * infile = fopen(argv[1], "r"); //Open *.asm for read-only
        FILE * outfile = fopen(argv[2], "w"); //Open *.hex for write-mode

        if (!infile) {
            exit(OTHER_ERROR); //Unable to open *.asm file
        }
        else if (!outfile) {
            exit(OTHER_ERROR); //Unable to open *.hex file
        }
        else {
            /* Begin assembly if expected number of arguments is correct */
            errorp = assemble(infile, outfile);
            fclose(infile);
            fclose(outfile);
        }
    }
    else {
        exit(OTHER_ERROR); //Missing commandline arguements
    }
    exit(errorp);
}

/** +
 * @fn enum errorCode assemble(FILE*, FILE*)
 * @brief Start of assembly process
 *
 * @param in            Input FILE* containing assembly code <*.asm>
 * @param out           Output FILE* containing hex code <*.hex>
 * @return errorCode    Any errors during the assembly process
 */
enum errorCode assemble(FILE * in, FILE * out ) {
    symbol * symbolTable = malloc(sizeof(struct symbol));
    instruction * instructionTable = malloc(sizeof(struct instruction));
    int tableCount = 0;
    int tableCount2 = 0;
    enum errorCode errorp;

    /* Create symbol table */
    errorp = createSymbolTable( in , START, & symbolTable, & tableCount, & instructionTable, & tableCount2);

    /* If no errors then proceed with printing/writing to file */
    if (errorp == OK_VALID) {
#ifdef Debug_Print_SymbolTable //Comment out MACRO definition to stop printing symbol table

        printf("\n\n\n");
        char * header = "/*******************Symbol Table*************************/";
        char * ender = "/********************************************************/";

        printWithIndent(header, INDENT_SYMBOL_);
        printf("\n");
        for (int j = 0; j < tableCount; j++) {
            printWithIndent("*", INDENT_SYMBOL_);
            char * addr = dec2hex(symbolTable[j].addr); //Convert base10 address to hex string
            printf("\t%-10s\t|\t%d(0x%s)\t\t*", symbolTable[j].symbolName, symbolTable[j].addr, addr);
            printf("\n");
            free(addr); // Free hex string
        }
        printWithIndent(ender, INDENT_SYMBOL_);
        printf("\n");
#endif

#ifdef Debug_Print_InstructionTable //Comment out MACRO definition to stop printing instruction table
        printf("\n\n\n");
        char * header1 = "/****************************************************Instruction Table*****************************************************************/";
        char * ender1 = "/**************************************************************************************************************************************/";

        printWithIndent(header1, INDENT_INSTRUCTION_);
        printf("\n");
        printWithIndent("*", INDENT_INSTRUCTION_);
        printf("\t%-10s\t|\t%-10s\t|\t%-10s\t|\t%-10s\t|\t%-10s\t\t%10s", "ADDRESS", "LABEL", "Directive", "Opcode", "Operands", "*");
        printf("\n");
        for (int j = 0; j < tableCount2; j++) {
            printWithIndent("*", INDENT_INSTRUCTION_);
            char * hex = dec2hex(instructionTable[j].addr); //Convert base10 address to hex string
            printf("\t%d (0x%s)\t", instructionTable[j].addr, hex);
            printf("\t%-10s\t\t%-10s\t\t%-10s\t\t", instructionTable[j].label, instructionTable[j].directive, instructionTable[j].opcode);
            int i = 0;
            for (; i < instructionTable[j].opCount; i++) {
                printf("%-5s\t", instructionTable[j].operands[i]);
                if (i < (instructionTable[j].opCount - 1))
                    printf(",");
            }
            for (; i < 3; i++)
                printf("\t");
            printf("%10s\n", "*");
            free(hex); // Free hex string
        }
        printWithIndent(ender1, INDENT_INSTRUCTION_);
        printf("\n");
#endif

        /* Iterate through instruction table entries to encode them */
        for (int j = 0; j < tableCount2; j++) {
            for (int i = 4; i < 35; i++) {
                /* Encode Directives */
                if (instructionTable[j].directive != NULL) {
                    if (strncmp(instructionTable[j].directive, inval[i], strlen(inval[i])) == 0 &&
                        strlen(instructionTable[j].directive) == strlen(inval[i])) {
                        errorp = encoder[i - 4]( & (instructionTable[j]), symbolTable, tableCount);
                    }
                }
                /* Encode Instructions */
                else if (instructionTable[j].opcode != NULL) {
                    if (strncmp(instructionTable[j].opcode, inval[i], strlen(inval[i])) == 0 &&
                        strlen(instructionTable[j].opcode) == strlen(inval[i])) {
                        errorp = encoder[i - 4]( & (instructionTable[j]), symbolTable, tableCount);
                    }
                }
            }
#ifdef Debug_Print_InstructionTable
            printf("%d\t", errorp);
#endif
            if (errorp == OK_VALID) {
                char * enc = NULL;
                if (instructionTable[j].encoding != NULL) {
                    enc = bin2hex((instructionTable[j].encoding));
                    prepend( & enc, "0x");
                    fprintf(out, "%s\n", enc);
                }
#ifdef Debug_Print_InstructionTable
                printf("%s", enc);
                printf("\n");
#endif
                free(enc);
            }
            else
                break;
        }
    }
    freeSymbolTable( & symbolTable, & tableCount); //Free the symbol table
    freeInstructionTable( & instructionTable, & tableCount2); //Free the instruction table

    /* return the errorcode
     * Should be OK_VALID if no errors */
    return errorp;
}

/** +
 * @fn void printWithIndent(char*, int)
 * @brief Print with appropriate indentation
 *
 * @param text   char* to print
 * @param indent Amount of indentation
 */
void printWithIndent(char * text, int indent) {
    printf("%*s%s", indent, "", text);
}

/** +
 * @fn void toUpperCase(char*)
 * @brief Standardize output to stdout by using only uppercase
 *
 * @param str char* to convert to uppercase
 */
void toUpperCase(char * str) {
    for (int i = 0; i < strlen(str); i++) {
        /* Check if alphabet before converting to uppercase */
        if (isalpha(str[i]) != 0)
            str[i] = toupper(str[i]);
    }
}

/** +
 * @fn enum errorCode createSymbolTable(FILE*, enum FSM, symbol**, int*, instruction**, int*)
 * @brief Creates a table of all symbols encountered during the first pass of the assembly code
 *
 * @param in                FILE* the assembly code file <*.asm>
 * @param state             State of the assembly process
 * @param symbolTable       Struct to hold the symbol table
 * @param tableCount        Number of elements/symbols in symbol table
 * @param instructionTable  Struct to hold instructions in the assembly code
 * @param tableCount2       Number of elements/instructions in instruction table
 * @return
 */
enum errorCode createSymbolTable(FILE * in , enum FSM state, symbol ** symbolTable, int * tableCount, instruction ** instructionTable, int * tableCount2) {
    char * line = NULL;
    size_t len = 0;
    bool flag = true;
    enum errorCode errorp;
    int index = 0;
    char * startAddr=NULL;

    while (flag) {
        switch (state) {
        case START:
            if (getline( & line, & len, in ) == -1)
                state = STOP;
            else {
	      char ** tokens;
	      int len1 = 0;
	      lexer(line, & tokens, & len1);
	      if (len1 != 0) {
		enum pFSM * stateT;
		errorp = integrityCheck( & tokens, & len1, & stateT);
		if (errorp == OK_VALID) {
		  if (stateT[0] == ORIG) {
		    state = PSTART;
		    startAddr = dec2dec(dec2dec2(tokens[1]));
		    insertInstruction(instructionTable, tableCount2, & tokens, stateT, len1, 0, startAddr);
		  }
		  else {
		    state = STOP;
		  }
		}
		else {
		  state = STOP;
		}
		free(stateT);
		freeLexemes( & tokens, & len1);
	      }
            }
            break;

        case PSTART:
            if (getline( & line, & len, in ) == -1)
	      state = STOP;
            else {
	      char ** tokens;
	      int len1 = 0;
	      lexer(line, & tokens, & len1);
	      if (len1 != 0) {
		index++;
		enum pFSM * stateT;
		errorp = integrityCheck( & tokens, & len1, & stateT);
		if (errorp == OK_VALID) {
		  if (stateT[0] == ORIG) {
		    state = STOP;
		  }
		  else if (stateT[0] == LABEL) {
		    insertSymbol(symbolTable, tableCount, tokens[0], index, startAddr);
		  }
		  else if (stateT[0] == END || stateT[1] == END) {
		    state = PEND;
		  }
		  insertInstruction(instructionTable, tableCount2, & tokens, stateT, len1, index - 1, startAddr);
		}
		else {
		  state = STOP;
		}
		free(stateT);
		freeLexemes( & tokens, & len1);
	      }
            }
            break;

        case PEND:
            free(startAddr);
            flag = false;
            break;

        case STOP:
        if(startAddr)
	  free(startAddr);
	errorp = errorp == OK_VALID ? OTHER_ERROR : errorp;
	flag = false;
	break;
        }
    }

    if (line) {
      free(line);
    }
    rewind( in );
    return errorp;
}

/** +
 * @fn void insertSymbol(symbol**, int*, char*, int, char*)
 * @brief Inserts a symbol into the symbol table
 *
 * @param symbolTable   The symbol table
 * @param tableCount    Number of elements/symbols in the symbol table
 * @param label         The symbol name
 * @param line          The line in assembly code where the symbol was defined
 * @param startAddr     Starting Address of the assemby code (defined using ORIG)
 * @return void
 */
void insertSymbol(symbol ** symbolTable, int * tableCount, char * label, int line, char * startAddr) {
    if ( * tableCount > 0) {
        int j = 0;

        /* Check if symbol exists in symbol table
         * LABELS(Symbols) can be defined somewhere and referenced elesewhere
         * We don't want to add duplicate entries*/
        for (; j < * tableCount; j++) {
            if (strncmp(( * symbolTable)[j].symbolName, label, strlen(label)) == 0) {
                break;
            }
        }

        /* If symbol/label not found in symbol table then add to symbol table*/
        if (j == * tableCount) {

            /* Increase memory allocation to symbol table to insert new symbol */
            * symbolTable = realloc( * symbolTable, (j + 1) * sizeof(struct symbol));
            ( * symbolTable)[j].symbolName = malloc(strlen(label) + 1);

            /* Copy label into symbol table */
            strncpy(( * symbolTable)[j].symbolName, label, (size_t)(strlen(label) + 1));

            /* Copy address(base10) into symbol table */
            ( * symbolTable)[j].addr = dec2dec2(startAddr) + (line - 1) * 2;
            * tableCount = ( * tableCount) + 1;
        }
    }
    else {
        /* Allocate memory to hold the label name*/
        ( * symbolTable)[0].symbolName = malloc(strlen(label) + 1);

        /* Copy label into symbol table */
        strncpy(( * symbolTable)[0].symbolName, label, (size_t)(strlen(label) + 1));

        /* Copy address(base10) into symbol table */
        ( * symbolTable)[0].addr = dec2dec2(startAddr) + (line - 1) * 2;
        * tableCount = ( * tableCount) + 1;
    }
}

/**+
 * @fn void insertInstruction(instruction**, int*, char***, enum pFSM*, int, int, char*)
 * @brief Inserts an instrcution into the instruction table
 *
 * @param instructionTable  The instruction table
 * @param tableCount        Number of elements/instructions in the instruction table
 * @param tokens            Tokens output after lexical analysis
 * @param stateTransition   Array of state transistions (see pTransitions)
 * @param step              Number of lexemes in the token
 * @param line              Line where the instruction was encountered
 * @param startAddr         Starting address of the code (defined using ORIG)
 * @return void
 */
void insertInstruction(instruction ** instructionTable, int * tableCount, char ** * tokens, enum pFSM * stateTransition, int step, int line, char * startAddr) {

    /* Increase memory allocation to insert new instruction
     * No checking necessary since every instruction has to be processed */
    ( * instructionTable) = realloc( * instructionTable, ( * tableCount + 1) * sizeof(struct instruction));
    ( * instructionTable)[ * tableCount].label = NULL;
    ( * instructionTable)[ * tableCount].directive = NULL;
    ( * instructionTable)[ * tableCount].opcode = NULL;
    ( * instructionTable)[ * tableCount].operands = NULL;
    ( * instructionTable)[ * tableCount].encoding = NULL;
    ( * instructionTable)[ * tableCount].opCount = 0;

    /* Find beginning indices of lexemes in tokens */
    int labelIndex = firstInstanceofLabel(stateTransition, step);
    int directiveIndex = firstInstanceofDirective(stateTransition, step);
    int opcodeIndex = firstInstanceofOpcode(stateTransition, step);
    int operandIndex = firstInstanceofOperands(stateTransition, step);

    /* Copy label lexeme if it exists into instruction table
     * 1. Label here refers to definition and not reference
     * 2. Label references are stored as operands
     *
     *      inst1-> START ADD R1, R1, #-1
     *      inst2-> BR START
     *
     * Label START in inst1 is a definition and hence will be stored as label in instruction table
     * Label START in inst2 is an operand/reference and hence will be stored as operands in instruction table
     */
    if (labelIndex == 0) {
        ( * instructionTable)[ * tableCount].label = malloc(strlen(( * tokens)[0]) + 1);
        strncpy(( * instructionTable)[ * tableCount].label, ( * tokens)[0], (size_t)(strlen(( * tokens)[0]) + 1));
    }

    /* Copy directive lexeme if it exists into instruction table */
    if (directiveIndex < step) {
        ( * instructionTable)[ * tableCount].directive = malloc(strlen(( * tokens)[directiveIndex]) + 1);
        strncpy(( * instructionTable)[ * tableCount].directive, ( * tokens)[directiveIndex], (size_t)(strlen(( * tokens)[directiveIndex]) + 1));
    }

    /* Copy opcode lexeme if it exists into instruction table */
    if (opcodeIndex < step) {
        ( * instructionTable)[ * tableCount].opcode = malloc(strlen(( * tokens)[opcodeIndex]) + 1);
        strncpy(( * instructionTable)[ * tableCount].opcode, ( * tokens)[opcodeIndex], (size_t)(strlen(( * tokens)[opcodeIndex]) + 1));
    }

    /* Copy operand lexemes if it exists into instruction table */
    if (operandIndex < step) {
        ( * instructionTable)[ * tableCount].operands = malloc((step - operandIndex) * sizeof(char ** ));

        for (int j = operandIndex; j < step; j++) {
            ( * instructionTable)[ * tableCount].operands[j - operandIndex] = malloc(strlen(( * tokens)[j]) + 1);
            strncpy(( * instructionTable)[ * tableCount].operands[j - operandIndex], ( * tokens)[j], (size_t)(strlen(( * tokens)[j]) + 1));
        }
        ( * instructionTable)[ * tableCount].opCount = step - operandIndex;
    }

    /* Copy instruction address into instruction table */
    if (startAddr != NULL) {
        ( * instructionTable)[ * tableCount].addr = dec2dec2(startAddr) + line * 2;
    }
    * tableCount = * tableCount + 1;
}

/** +
 * @fn enum errorCode encodeORIG(instruction*, symbol*, int)
 * @brief Encode the ORIG directive
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeORIG(instruction * instruct, symbol * symbol, int count) {
    if ((( * instruct).addr) % 2 != 0)
        return INVALID_CONSTANT;

    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return INVALID_CONSTANT;

    ( * instruct).encoding = dec2bin(( * instruct).addr);
    return OK_VALID;
}

/** +
 * @fn enum errorCode encodeFILL(instruction*, symbol*, int)
 * @brief Encode the FILL directive
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeFILL(instruction * instruct, symbol * symbol, int count) {

    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int constant = strtol((*instruct).operands[0] + 1, NULL, 10);

    if(constant < -32768 || constant > 65535)
        return INVALID_CONSTANT;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));

    strcat(str, dr);
    str[16] = '\0';
    free(dr);

    ( * instruct).encoding = str;
    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeEND(instruction*, symbol*, int)
 * @brief Encode the END directive
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeEND(instruction * instruct, symbol * symbol, int count) {
    return OK_VALID;
}

/** +
 * @fn enum errorCode encodeNOP(instruction*, symbol*, int)
 * @brief Encode the NOP instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeNOP(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));
    strcat(str, "0000000000000000");
    str[16] = '\0';
    ( * instruct).encoding = str;

    return OK_VALID;
}

/** +
 * @fn enum errorCode encodeTRAP(instruction*, symbol*, int)
 * @brief Encode the TRAP instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeTRAP(instruction * instruct, symbol * symbol, int count) {

    int op = strtol(( * instruct).operands[0] + 1, NULL, 10);

    if(op<0 || op > 255)
    return INVALID_CONSTANT;


    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));

    strcat(str, "11110000");
    strcat(str, dr + 8);
    str[16] = '\0';
    free(dr);
    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeHALT(instruction*, symbol*, int)
 * @brief Encode the HALT instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeHALT(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    strcat(str, "1111000000100101");
    str[16] = '\0';

    ( * instruct).encoding = str;

    return OK_VALID;
}

/** +
 * @fn enum errorCode encodeADD(instruction*, symbol*, int)
 * @brief Encode the ADD instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeADD(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0001");
    strcat(str, dr + 13);
    strcat(str, sr + 13);
    char * sr1 = NULL;

    if (checkImmidiate( & ( * instruct).operands[2]) == IMM) {
        strcat(str, "1");
        if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 5)) {
            free(dr);
            free(sr);
            free(str);
            return INVALID_CONSTANT;
        }
        sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
        strcat(str, sr1 + 11);
    } else {
        strcat(str, "000");
        sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
        strcat(str, sr1 + 13);
    }
    str[16] = '\0';
    free(dr);
    free(sr);
    free(sr1);
    ( * instruct).encoding = str;

    return OK_VALID;
}

/** +
 * @fn enum errorCode encodeXOR(instruction*, symbol*, int)
 * @brief Encode the XOR instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeXOR(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "1001");
    strcat(str, dr + 13);
    strcat(str, sr + 13);
    char * sr1 = NULL;
    if (checkImmidiate( & ( * instruct).operands[2]) == IMM) {
        strcat(str, "1");
        if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 5)) {
            free(dr);
            free(sr);
            free(str);
            return INVALID_CONSTANT;
        }
        sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
        strcat(str, sr1 + 11);
    } else {
        strcat(str, "000");
        sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
        strcat(str, sr1 + 13);
    }
    str[16] = '\0';
    free(dr);
    free(sr);
    free(sr1);
    ( * instruct).encoding = str;

    return OK_VALID;
}

/** +
 * @fn enum errorCode encodeAND(instruction*, symbol*, int)
 * @brief Encode the AND instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeAND(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0101");
    strcat(str, dr + 13);
    strcat(str, sr + 13);

    char * sr1 = NULL;
    if (checkImmidiate( & ( * instruct).operands[2]) == IMM) {
        strcat(str, "1");
        if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 5)) {
            free(dr);
            free(sr);
            free(str);
            return INVALID_CONSTANT;
        }
        sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
        strcat(str, sr1 + 11);
    } else {
        strcat(str, "000");
        sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
        strcat(str, sr1 + 13);
    }
    str[16] = '\0';
    free(dr);
    free(sr);
    free(sr1);
    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeJMP(instruction*, symbol*, int)
 * @brief Encode the JMP instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeJMP(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));

    strcat(str, "1100000");
    strcat(str, dr + 13);
    strcat(str, "000000");
    str[16] = '\0';
    free(dr);
    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeRET(instruction*, symbol*, int)
 * @brief Encode the RET instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeRET(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    strcat(str, "1100000111000000");
    str[16] = '\0';

    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeJSR(instruction*, symbol*, int)
 * @brief Encode the JSR instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeJSR(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[0]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;

        strcat(str, "01001");
        if (!checkValidRange(offset, 11)) {
            free(str);
            return OTHER_ERROR;
        }
        char * dr = dec2bin(offset);
        strcat(str, dr + 5);
        str[16] = '\0';
        free(dr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;

}

/** +
 * @fn enum errorCode encodeBR(instruction*, symbol*, int)
 * @brief Encode the BR instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeBR(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[0]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;
        if (!checkValidRange(offset, 9)) {
            free(str);
            return OTHER_ERROR;
        }
        strcat(str, "0000");

        strcat(str, "111");

        char * dr = dec2bin(offset);
        strcat(str, dr + 7);
        str[16] = '\0';
        free(dr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;
}

/** +
 * @fn enum errorCode encodeBRN(instruction*, symbol*, int)
 * @brief Encode the BRN instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeBRN(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[0]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;
        if (!checkValidRange(offset, 9)) {
            free(str);
            return OTHER_ERROR;
        }
        strcat(str, "0000");

        strcat(str, "100");

        char * dr = dec2bin(offset);
        strcat(str, dr + 7);
        str[16] = '\0';
        free(dr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;
}

/** +
 * @fn enum errorCode encodeBRZ(instruction*, symbol*, int)
 * @brief Encode the BRZ instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeBRZ(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[0]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;
        if (!checkValidRange(offset, 9)) {
            free(str);
            return OTHER_ERROR;
        }
        strcat(str, "0000");

        strcat(str, "010");

        char * dr = dec2bin(offset);
        strcat(str, dr + 7);
        str[16] = '\0';
        free(dr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;
}

/** +
 * @fn enum errorCode encodeBRP(instruction*, symbol*, int)
 * @brief Encode the BRP instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeBRP(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[0]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;
        if (!checkValidRange(offset, 9)) {
            free(str);
            return OTHER_ERROR;
        }
        strcat(str, "0000");

        strcat(str, "001");

        char * dr = dec2bin(offset);
        strcat(str, dr + 7);
        str[16] = '\0';
        free(dr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;
}

/** +
 * @fn enum errorCode encodeBRNZ(instruction*, symbol*, int)
 * @brief Encode the BRNZ instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeBRNZ(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[0]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;
        if (!checkValidRange(offset, 9)) {
            free(str);
            return OTHER_ERROR;
        }
        strcat(str, "0000");

        strcat(str, "110");

        char * dr = dec2bin(offset);
        strcat(str, dr + 7);
        str[16] = '\0';
        free(dr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;
}

/** +
 * @fn enum errorCode encodeBRNP(instruction*, symbol*, int)
 * @brief Encode the BRNP instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeBRNP(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[0]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;
        if (!checkValidRange(offset, 9)) {
            free(str);
            return OTHER_ERROR;
        }
        strcat(str, "0000");

        strcat(str, "101");

        char * dr = dec2bin(offset);
        strcat(str, dr + 7);
        str[16] = '\0';
        free(dr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;
}

/** +
 * @fn enum errorCode encodeBRZP(instruction*, symbol*, int)
 * @brief Encode the BRZP instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeBRZP(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[0]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;
        if (!checkValidRange(offset, 9)) {
            free(str);
            return OTHER_ERROR;
        }
        strcat(str, "0000");

        strcat(str, "011");

        char * dr = dec2bin(offset);
        strcat(str, dr + 7);
        str[16] = '\0';
        free(dr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;
}

/** +
 * @fn enum errorCode encodeBRNZP(instruction*, symbol*, int)
 * @brief Encode the BRNZP instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeBRNZP(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[0]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;
        if (!checkValidRange(offset, 9)) {
            free(str);
            return OTHER_ERROR;
        }
        strcat(str, "0000");

        strcat(str, "111");

        char * dr = dec2bin(offset);
        strcat(str, dr + 7);
        str[16] = '\0';
        free(dr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;
}

/** +
 * @fn enum errorCode encodeJSRR(instruction*, symbol*, int)
 * @brief Encode the JSRR instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeJSRR(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));

    strcat(str, "0100");
    strcat(str, "000");
    strcat(str, dr + 13);
    strcat(str, "000000");
    str[16] = '\0';
    free(dr);
    ( * instruct).encoding = str;

    return OK_VALID;
}

/** +
 * @fn enum errorCode encodeLDB(instruction*, symbol*, int)
 * @brief Encode the LDB instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeLDB(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0010");
    strcat(str, dr + 13);
    strcat(str, sr + 13);

    if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 6)) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }
    char * sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
    strcat(str, sr1 + 10);
    str[16] = '\0';
    free(dr);
    free(sr);
    free(sr1);
    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeLDW(instruction*, symbol*, int)
 * @brief Encode the LDW instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeLDW(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0110");
    strcat(str, dr + 13);
    strcat(str, sr + 13);

    if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 6)) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }
    char * sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
    strcat(str, sr1 + 10);

    str[16] = '\0';
    free(dr);
    free(sr);
    free(sr1);

    ( * instruct).encoding = str;
    return OK_VALID;
}

/** +
 * @fn enum errorCode encodeLEA(instruction*, symbol*, int)
 * @brief Encode the LEA instruction
 *
 * @param instruct
 * @param sym
 * @param count
 * @return errorCode
 */
enum errorCode encodeLEA(instruction * instruct, symbol * sym, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    int addrLabel = getAddrofLabel(sym, count, ( * instruct).operands[1]);

    if (addrLabel != -1) {
        char * str = calloc(17, sizeof(char));
        int offset = (addrLabel - (( * instruct).addr + 2)) / 2;

        if (!checkValidRange(offset, 9)) {
            free(str);
            return OTHER_ERROR;
        }

        strcat(str, "1110");
        char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
        char * sr = dec2bin(offset);
        strcat(str, dr + 13);
        strcat(str, sr + 7);
        str[16] = '\0';
        free(dr);
        free(sr);
        ( * instruct).encoding = str;

        return OK_VALID;
    } else
        return UNDEFINED_LABEL;

}

/** +
 * @fn enum errorCode encodeNOT(instruction*, symbol*, int)
 * @brief Encode the NOT instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeNOT(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "1001");
    strcat(str, dr + 13);
    strcat(str, sr + 13);
    strcat(str, "111111");
    str[16] = '\0';
    free(dr);
    free(sr);
    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeRTI(instruction*, symbol*, int)
 * @brief Encode the RIT instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeRTI(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    strcat(str, "1000000000000000");
    str[16] = '\0';

    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeLSHF(instruction*, symbol*, int)
 * @brief Encode the SHF instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeLSHF(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "1101");
    strcat(str, dr + 13);
    strcat(str, sr + 13);
    strcat(str, "00");

    if (strtol(( * instruct).operands[2] + 1, NULL, 10) > 15 ||
    strtol(( * instruct).operands[2] + 1, NULL, 10) < 0) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }

    char * sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
    strcat(str, sr1 + 12);
    str[16] = '\0';
    free(dr);
    free(sr);
    free(sr1);
    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeRSHFL(instruction*, symbol*, int)
 * @brief Encode the RSHFL instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeRSHFL(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "1101");
    strcat(str, dr + 13);
    strcat(str, sr + 13);
    strcat(str, "01");

    if (strtol(( * instruct).operands[2] + 1, NULL, 10) > 15 ||
    strtol(( * instruct).operands[2] + 1, NULL, 10) < 0) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }

    char * sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
    strcat(str, sr1 + 12);
    str[16] = '\0';
    free(dr);
    free(sr);
    free(sr1);
    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeRSHFA(instruction*, symbol*, int)
 * @brief Encode the RSHFA instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeRSHFA(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "1101");
    strcat(str, dr + 13);
    strcat(str, sr + 13);
    strcat(str, "11");

    if (strtol(( * instruct).operands[2] + 1, NULL, 10) > 15 ||
    strtol(( * instruct).operands[2] + 1, NULL, 10) < 0) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }

    char * sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
    strcat(str, sr1 + 12);
    str[16] = '\0';
    free(dr);
    free(sr);
    free(sr1);
    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeSTB(instruction*, symbol*, int)
 * @brief Encode the STB instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeSTB(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0011");
    strcat(str, dr + 13);
    strcat(str, sr + 13);

    if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 6)) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }

    char * sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
    strcat(str, sr1 + 10);
    str[16] = '\0';
    free(sr);
    free(dr);
    free(sr1);
    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn enum errorCode encodeSTW(instruction*, symbol*, int)
 * @brief Encode the STW instruction
 *
 * @param instruct
 * @param symbol
 * @param count
 * @return errorCode
 */
enum errorCode encodeSTW(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = dec2bin(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = dec2bin(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0111");
    strcat(str, dr + 13);
    strcat(str, sr + 13);

    if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 6)) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }

    char * sr1 = dec2bin(strtol(( * instruct).operands[2] + 1, NULL, 10));
    strcat(str, sr1 + 10);
    str[16] = '\0';
    free(sr);
    free(dr);
    free(sr1);

    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn void freeLexemes(char***, int*)
 * @brief Free memory allocated to store lexemes
 *
 * @param lexemes
 * @param len
 * @return void
 */
void freeLexemes(char ** * lexemes, int * len) {
    for (int i = 0; i < * len; i++) {
        free(( * lexemes)[i]);
    }

    free( * lexemes);
}

/** +
 * @fn void freeSymbolTable(symbol**, int*)
 * @brief Free the symbol table
 *
 * @param symbolTable
 * @param tableCount
 * @return void
 */
void freeSymbolTable(symbol ** symbolTable, int * tableCount) {

    for (int j = 0; j < * tableCount; j++) {
        free(( * symbolTable)[j].symbolName);
    }

    free( * symbolTable);
}

/** +
 * @fn void freeInstructionTable(instruction**, int*)
 * @brief Free the instruction table
 *
 * @param instructionTable
 * @param tableCount
 * @return void
 */
void freeInstructionTable(instruction ** instructionTable, int * tableCount) {
    for (int j = 0; j < * tableCount; j++) {
        free(( * instructionTable)[j].label);
        free(( * instructionTable)[j].directive);
        free(( * instructionTable)[j].opcode);

        for (int i = 0; i < ( * instructionTable)[j].opCount; i++)
            free(( * instructionTable)[j].operands[i]);

        free(( * instructionTable)[j].operands);
        free(( * instructionTable)[j].encoding);

    }

    free( * instructionTable);
}

/** +
 * @fn void lexer(char*, char***, int*)
 * @brief Lex the line extracted from the input <*.asm> file into tokens
 *
 * @param line
 * @param tokens
 * @param len
 * @return void
 */
void lexer(char * line, char ** * tokens, int * len) {

#ifdef Debug_Print
    printf("\n%s\n", line);
#endif

    size_t index = strcspn(line, ";");

    if (index != 0) //Ignore comment lines
    {

        /********************************Tokenize, Parse for Integrity check and create Symbol Table**************/

        char * subbuff = malloc(index + 1);
        strncpy(subbuff, &line[0], index);
    subbuff[index] = '\0';

#ifdef Debug_Print
        printf("%s\n", subbuff);
#endif

        char * token = strtok(subbuff, " ,\n\t");
        //*tokens = malloc(sizeof(char*));
        int c = 0;

        while (token != NULL) {
            * tokens = c == 0 ? malloc(sizeof(char * )) : realloc( * tokens, (c + 1) * sizeof(char * ));
            ( * tokens)[c] = malloc((strlen(token) + 1) * sizeof(char));
            strncpy(( * tokens)[c], token, (size_t)(strlen(token) + 1));
            toUpperCase(( * tokens)[c]);
            token = strtok(NULL, " ,\n\t");
            c++;
        }

        * len = c;

        /***********************Cleanup Buffs***************************/
        free(subbuff);
        /***************************************************************/

        /*************************************************************************************************/
    }

}

/** +
 * @fn int hex2dec(char*)
 * @brief Convert hex string into Base10 number
 *
 * @param str
 * @return int
 */
int hex2dec(char * str) {
    int dec = INT_MAX;
    bool valid = true;

    if (strlen(str) >= 2) {
        if (str[0] == 'X') {
            for (int i = 1; i < strlen(str); i++) {
                if (!isxdigit(str[i])) {
                    if (!((str[i] == '-' || str[i] == '+') && i == 1)) {
                        valid = false;
                        break;
                    }
                }
            }

            dec = valid ? strtol((str + 1), NULL, 16) : dec;
        }
    }

    return dec;
}

/** +
 * @fn char dec2hex*(int)
 * @brief Convert a Base10 number into hex string
 *
 * @param dec
 * @return char*
 */
char * dec2hex(int dec) {
    char * hex = malloc(sizeof(char * ));

    sprintf(hex, "%.4X", dec);

    return hex;
}

/** +
 * @fn void prepend(char**, char*)
 * @brief
 *
 * @param str
 * @param p
 * @return void
 */
void prepend(char ** str, char * p) {
    char * str2 = calloc(strlen( * str) + strlen(p) + 1, sizeof(char));

    strcat(str2, p);
    strcat(str2, * str);
    str2[strlen( * str) + strlen(p)] = '\0';
    free( * str);
    * str = str2;
}

/** +
 * @fn char bin2hex*(char*)
 * @brief Convert Base2 string into hex string
 *
 * @param dec
 * @return char*
 */
char * bin2hex(char * dec) {

    int dec2 = strtol(dec, NULL, 2);

    return dec2hex(dec2);
}

/** +
 * @fn char dec2bin*(int)
 * @brief Convert Base10 number into Base2 string
 *
 * @param dec
 * @return char*
 */
char * dec2bin(int dec) {
    char * dst = calloc(17, sizeof(char));

    dec = dec >= 0 ? dec : (65536 + dec);
    for (int i = 0; i <= 15; i++) {
        dst[15 - i] = ((dec >> i) & 1) ? '1' : '0';
    }

    dst[16] = '\0';

    return dst;
}

/** +
 * @fn char dec2dec*(int)
 * @brief Convert Base10 number into Base10 string
 *
 * @param dec
 * @return char*
 */
char * dec2dec(int dec) {
    char * dst = int2str(dec);
    prepend( & dst, "#");
    return dst;
}

/** +
 * @fn int dec2dec2(char*)
 * @brief Convert Base10 string into Base10 number
 *
 * @param dec
 * @return int
 */
int dec2dec2(char * dec) {
    if (isValidDec(dec))
        return strtol((dec + 1), NULL, 10);
    else
        return INT_MAX;
}

/** +
 * @fn char int2str*(int)
 * @brief
 *
 * @param dec
 * @return char*
 */
char * int2str(int dec) {
    char * str = malloc(sizeof(char * ));

    sprintf(str, "%d", dec);

    return str;
}

/** +
 * @fn bool isValidHex(char*)
 * @brief CHeck if Base16/hex string is a valid Base16 number
 *
 * @param str
 * @return bool
 */
bool isValidHex(char * str) {

    bool valid = false;

    if (strlen(str) >= 2) {
        if (str[0] == 'X') {
            if (hex2dec(str) != INT_MAX)
                valid = true;
        }
    }

    return valid;
}

/** +
 * @fn bool isValidDec(char*)
 * @brief Check if a Base10 string is a valid Base10 number
 *
 * @param str
 * @return bool
 */
bool isValidDec(char * str) {

    bool valid = false;

    if (str[0] == '#') {
        for (int j = 1; j < strlen(str); j++) {
            if (isdigit(str[j]) == 0) {
                if (((str[j] == '-' || str[j] == '+') && j == 1)) {
                    valid = true;
                } else {
                    valid = true;
                    break;
                }
            }
            valid = true;
        }
    }

    return valid;
}

/** +
 * @fn enum pFSM checkLabel(char*)
 * @brief Check if Label defined is valid
 *
 * @param str
 * @return pFSM
 */
enum pFSM checkLabel(char * str) {
    bool flag = true;

    if (strlen(str) > 0) {
        if (str[0] != 'X') {
            for (int j = 0; j < strlen(str); j++) {
                flag &= (isalnum(str[j]) != 0);
            }

            for (int j = 0; j < 43; j++) {
                flag &= ((strncmp(str, inval[j], strlen(str)) == 0 && strlen(str) == strlen(inval[j])) ? false : true);
            }
        } else
            flag = false;
    } else
        flag = false;

    return flag ? LABEL : INVALID;
}

/** +
 * @fn enum pFSM checkpOP(char*)
 * @brief Check if directive is valid
 *
 * @param str
 * @return pFSM
 */
enum pFSM checkpOP(char * str) {
    if (strncmp(str, ".ORIG", strlen(str)) == 0 && (strlen(str) == strlen(".ORIG")))
        return ORIG;

    else if (strncmp(str, ".FILL", strlen(str)) == 0 && (strlen(str) == strlen(".FILL")))
        return FILL;

    else if (strncmp(str, ".END", strlen(str)) == 0 && (strlen(str) == strlen(".END")))
        return END;

    else
        return INVALID;
}

/** +
 * @fn enum pFSM checkInst(char*)
 * @brief Check if opcode used exists in ISA
 *
 * @param str
 * @return pFSM
 */
enum pFSM checkInst(char * str) {
    enum pFSM op = INVALID;

    if (strlen(str) > 0) {
        for (int j = 0; j < 28; j++) {
            if (strncmp(str, inval[j + 7], strlen(str)) == 0) {
                if (strlen(str) == strlen(inval[j + 7])) {
                    op = pStates[j + 5];
                    break;
                }
            }
        }
    }
    return op;
}

/** +
 * @fn enum pFSM checkRegister(char*)
 * @brief CHeck if registers used are allowed
 *
 * @param str
 * @return pFSM
 */
enum pFSM checkRegister(char * str) {
    enum pFSM op = INVALID;

    if (strlen(str) == 2) {
        for (int j = 0; j < 8; j++) {
            if (strncmp(str, inval[j + 35], strlen(str)) == 0) {
                if (strlen(str) == strlen(inval[j + 35])) {
                    op = REG;
                    break;
                }
            }
        }
    }
    return op;
}

/** +
 * @fn enum pFSM checkImmidiate(char**)
 * @brief Check if immediate values used are valid
 *
 * @param str
 * @return pFSM
 */
enum pFSM checkImmidiate(char ** str) {
    enum pFSM op = INVALID;

    if (strlen( * str) >= 2) {
        if (isValidHex( * str)) {
            int dec = hex2dec( * str);
            free( * str);
            * str = int2str(dec);
            prepend(str, "#");
            op = IMM;
        }
        else if (isValidDec( * str)) {
            op = IMM;
        }
    }
    return op;
}

/** +
 * @fn enum errorCode integrityCheck(char***, int*, enum pFSM**)
 * @brief
 *
 * @param tokens
 * @param count
 * @param stateTransition
 * @return errorCode
 */
enum errorCode integrityCheck(char ** * tokens, int * count, enum pFSM ** stateTransition) {

    int step = 0;
    bool flag;
    enum pFSM state = START;

    *stateTransition = malloc(sizeof(enum pFSM));

    while (step < *count) {
        *stateTransition = realloc(*stateTransition,
                                   (step + 1) * sizeof(enum pFSM));
        state = checkLabel((*tokens)[step]);
        state = state == INVALID ? checkpOP((*tokens)[step]) : state;
        state = state == INVALID ? checkInst((*tokens)[step]) : state;
        state = state == INVALID ? checkRegister((*tokens)[step]) : state;
        state = state == INVALID ? checkImmidiate(&((*tokens)[step])) : state;

        (*stateTransition)[step] = state;

        if (state == TRAP) {
            if (step < *count) {
                if (!isValidHex((*tokens)[step + 1])) {
                    return INVALID_CONSTANT;
                }
            }
        }
        step++;
    }

    for (int i = 0; i < 68; i++) {

        flag = true;
        int j = 0;

        for (; j < step; j++) {
            flag &= ((*stateTransition)[j] == pTransitions[i][j]);
        }

        if (flag) {

            flag &= (j < 5 ? (pTransitions[i][j] == IGN) : flag);
            if (flag)
                break;
        }
    }

    if (!flag) {
        int i = firstInstanceofOperands(*stateTransition,
                                        step);

        if (i == 0) {
            if ((*stateTransition)[0] != ORIG && (*stateTransition)[0] != FILL
                    && (*stateTransition)[0] != END) {
                return INVALID_OPCODE;
            }
        } else {
            if ((*stateTransition)[i - 1] == LABEL)
                return INVALID_OPCODE;
        }
        return OTHER_ERROR;
    }

#ifdef Debug_Print
    if (flag)
        printf("True\n");
    else
        printf("False\n");
#endif

    return OK_VALID;
}

/** +
 * @fn int firstInstanceofLabel(enum pFSM*, int)
 * @brief
 *
 * @param states
 * @param len
 * @return int
 */
int firstInstanceofLabel(enum pFSM * states, int len) {
    int j = 0;
    for (; j < len; j++) {
        if (states[j] == LABEL) break;
    }
    return j;
}

/** +
 * @fn int firstInstanceofDirective(enum pFSM*, int)
 * @brief
 *
 * @param states
 * @param len
 * @return int
 */
int firstInstanceofDirective(enum pFSM * states, int len) {
    int j = 0;
    for (; j < len; j++) {
        if (states[j] == ORIG || states[j] == FILL || states[j] == END) break;
    }
    return j;
}

/** +
 * @fn int firstInstanceofOpcode(enum pFSM*, int)
 * @brief
 *
 * @param states
 * @param len
 * @return int
 */
int firstInstanceofOpcode(enum pFSM * states, int len) {
    int j = 0;
    for (; j < len; j++) {
        int i = 0;
        for (; i < 28; i++) {
            if (states[j] == pStates[i + 5]) break;
        }
        if (i != 28) break;
    }
    return j;
}

/** +
 * @fn int firstInstanceofOperands(enum pFSM*, int)
 * @brief
 *
 * @param states
 * @param len
 * @return int
 */
int firstInstanceofOperands(enum pFSM * states, int len) {
    int j = 0;
    for (; j < len; j++) {
        if (states[j] == REG || states[j] == IMM || (states[j] == LABEL && j > 0)) break;
    }
    return j;
}

/** +
 * @fn int getAddrofLabel(symbol*, int, char*)
 * @brief
 *
 * @param sym
 * @param count
 * @param label
 * @return int
 */
int getAddrofLabel(symbol * sym, int count, char * label) {
    int addr = -1;
    for (int j = 0; j < count; j++) {
        if (strlen(label) == strlen(sym[j].symbolName)) {
            if (strncmp(label, sym[j].symbolName, strlen(label)) == 0) {
                addr = sym[j].addr;
            }
        }
    }
    return addr;
}

/** +
 * @fn bool checkValidRange(int, int)
 * @brief
 *
 * @param dec
 * @param numBytes
 * @return bool
 */
bool checkValidRange(int dec, int numBytes) {
    return ((dec <= (pow(2, numBytes - 1) - 1)) && (dec >= -pow(2, numBytes - 1)));
}

