/*
Name 1: Aditya Thimmaiah
UTEID 1: at39896
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

//#define Debug_Print
#define Debug_Print_InstructionTable
#define Debug_Print_SymbolTable


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

enum FSM {
    START,
    PSTART,
    PEND,
    STOP
};

enum pFSM {
    LABEL,
    ORIG,
    FILL,
    END,
    ADD,
    AND,
    BR,
    BRN,
    BRZ,
    BRP,
    BRNZ,
    BRNP,
    BRZP,
    BRNZP,
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
    INVALID,
    REG,
    IMM,
    IGN
};

enum errorCode {
    OK_VALID,
    UNDEFINED_LABEL,
    INVALID_OPCODE,
    INVALID_CONSTANT,
    OTHER_ERROR
};

enum pFSM pTransitions[68][5] = {					{ORIG, IMM, IGN, IGN, IGN}		,		{LABEL, ORIG, IMM, IGN, IGN},
									{FILL, IMM, IGN, IGN, IGN}		, 		{LABEL, FILL, IMM, IGN, IGN},
									{END, IGN, IGN, IGN, IGN}		,		{LABEL, END, IGN, IGN, IGN},
									{ADD, REG, REG, IMM, IGN}		, 		{ADD, REG, REG, REG, IGN},
									{LABEL, ADD, REG, REG, IMM}		, 		{LABEL, ADD, REG, REG, REG},
									{AND, REG, REG, IMM, IGN}		, 		{AND, REG, REG, REG, IGN},
									{LABEL, AND, REG, REG, IMM}		, 		{LABEL, AND, REG, REG, REG},
									{XOR, REG, REG, IMM, IGN}		, 		{XOR, REG, REG, REG, IGN},
									{LABEL, XOR, REG, REG, IMM}		, 		{LABEL, XOR, REG, REG, REG},		
									{BR, LABEL, IGN, IGN, IGN}		, 		{LABEL, BR, LABEL, IGN, IGN},
									{BRN, LABEL, IGN, IGN, IGN}		, 		{LABEL, BRN, LABEL, IGN, IGN},
									{BRZ, LABEL, IGN, IGN, IGN}		, 		{LABEL, BRZ, LABEL, IGN, IGN},
									{BRP, LABEL, IGN, IGN, IGN}		, 		{LABEL, BRP, LABEL, IGN, IGN},
									{BRNZ, LABEL, IGN, IGN, IGN}		, 		{LABEL, BRNZ, LABEL, IGN, IGN},
									{BRNP, LABEL, IGN, IGN, IGN}		, 		{LABEL, BRNP, LABEL, IGN, IGN},
									{BRZP, LABEL, IGN, IGN, IGN}		, 		{LABEL, BRZP, LABEL, IGN, IGN},
									{BRNZP, LABEL, IGN, IGN, IGN}		, 		{LABEL, BRNZP, LABEL, IGN, IGN},
									{JMP, REG, IGN, IGN, IGN}		, 		{LABEL, JMP, REG, IGN, IGN},
									{JSR, LABEL, IGN, IGN, IGN}		, 		{LABEL, JSR, LABEL, IGN, IGN},
									{JSRR, REG, IGN, IGN, IGN}		, 		{LABEL, JSRR, REG, IGN, IGN},
									{LDB, REG, REG, IMM, IGN}		, 		{LABEL, LDB, REG, REG, IMM},
									{LDW, REG, REG, IMM, IGN}		, 		{LABEL, LDW, REG, REG, IMM},
									{LEA, REG, LABEL, IGN, IGN}		, 		{LABEL, LEA, REG, LABEL, IGN},
									{NOT, REG, REG, IGN, IGN}		, 		{LABEL, NOT, REG, REG, IGN},
									{RET, IGN, IGN, IGN, IGN}		, 		{LABEL, RET, IGN, IGN, IGN},
									{RTI, IGN, IGN, IGN, IGN}		, 		{LABEL, RTI, IGN, IGN, IGN},
									{LSHF, REG, REG, IMM, IGN}		, 		{LABEL, LSHF, REG, REG, IMM},
									{RSHFL, REG, REG, IMM, IGN}		, 		{LABEL, RSHFL, REG, REG, IMM},
									{RSHFA, REG, REG, IMM, IGN}		, 		{LABEL, RSHFA, REG, REG, IMM},
									{STB, REG, REG, IMM, IGN}		, 		{LABEL, STB, REG, REG, IMM},
									{STW, REG, REG, IMM, IGN}		, 		{LABEL, STW, REG, REG, IMM},
									{TRAP, IMM, IGN, IGN, IGN}		,		{LABEL, TRAP, IMM, IGN, IGN},
									{HALT, IGN, IGN, IGN, IGN}		,		{LABEL, HALT, IGN, IGN, IGN},
									{NOP, IGN, IGN, IGN, IGN}		, 		{LABEL, NOP, IGN, IGN, IGN}
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

typedef struct symbol {
    char * symbolName;
    int addr;
}
symbol;

typedef struct instruction {
    char * label;
    char * directive;
    char * opcode;
    char ** operands;
    char * encoding;
    int addr;
    int opCount;
}
instruction;



/********************************Lexing Methods******************************************/
enum errorCode assemble(FILE *, FILE* );
void lexer(char * , char ** * , int * );
enum errorCode createSymbolTable(FILE * , enum FSM, symbol ** , int * , instruction ** , int * );
void insertSymbol(symbol ** , int * , char * , int, char * );
void insertInstruction(instruction ** , int * , char ** * , enum pFSM * , int, int, char * );
/****************************************************************************************/



/********************************Data Conversion Methods*********************************/
int hex2dec(char * );
char * dec2hex(int);
char * dec2bin(int);
char * bin2hex(char * );
char * int2str(int);
bool isValidHex(char * );
bool isValidDec(char * );
char * dec2dec(int);
int dec2dec2(char * );
bool checkValidRange(int, int);
/***************************************************************************************/



/******************************Cleanup Methods******************************************/
void freeLexemes(char ** * , int * );
void freeSymbolTable(symbol ** , int * );
void freeInstructionTable(instruction ** , int * );
/***************************************************************************************/



/******************************Miscellaneous Methods************************************/
void printWithIndent(char * , int);
void prepend(char ** , char * );
int getAddrofLabel(symbol * , int, char * );
void toUpperCase(char * );
/***************************************************************************************/



/***************************************Encoder Methods*********************************/
enum errorCode encodeADD(instruction * , symbol * , int);
enum errorCode encodeAND(instruction * , symbol * , int);
enum errorCode encodeXOR(instruction * , symbol * , int);
enum errorCode encodeORIG(instruction * , symbol * , int);
enum errorCode encodeJMP(instruction * , symbol * , int);
enum errorCode encodeRET(instruction * , symbol * , int);
enum errorCode encodeJSRR(instruction * , symbol * , int);
enum errorCode encodeJSR(instruction * , symbol * , int);
enum errorCode encodeBR(instruction * , symbol * , int);
enum errorCode encodeBRN(instruction * , symbol * , int);
enum errorCode encodeBRZ(instruction * , symbol * , int);
enum errorCode encodeBRP(instruction * , symbol * , int);
enum errorCode encodeBRNZ(instruction * , symbol * , int);
enum errorCode encodeBRNP(instruction * , symbol * , int);
enum errorCode encodeBRZP(instruction * , symbol * , int);
enum errorCode encodeBRNZP(instruction * , symbol * , int);
enum errorCode encodeLDB(instruction * , symbol * , int);
enum errorCode encodeLDW(instruction * , symbol * , int);
enum errorCode encodeLEA(instruction * , symbol * , int);
enum errorCode encodeSTB(instruction * , symbol * , int);
enum errorCode encodeSTW(instruction * , symbol * , int);
enum errorCode encodeLSHF(instruction * , symbol * , int);
enum errorCode encodeRSHFL(instruction * , symbol * , int);
enum errorCode encodeRSHFA(instruction * , symbol * , int);
enum errorCode encodeRTI(instruction * , symbol * , int);
enum errorCode encodeFILL(instruction * , symbol * , int);
enum errorCode encodeEND(instruction * , symbol * , int);
enum errorCode encodeNOP(instruction * , symbol * , int);
enum errorCode encodeTRAP(instruction * , symbol * , int);
enum errorCode encodeHALT(instruction * , symbol * , int);
enum errorCode encodeNOT(instruction * , symbol * , int);
/***************************************************************************************/


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


/***************************************Lexeme Integrity Checker*********************************/
enum pFSM checkLabel(char * );
enum pFSM checkpOP(char * );
enum pFSM checkInst(char * );
enum pFSM checkRegister(char * );
enum pFSM checkImmidiate(char ** );
enum errorCode integrityCheck(char ** * , int * , enum pFSM ** );
/************************************************************************************************/



/***************************************Find Lexemes in StateTransitionTable*********************/
int firstInstanceofLabel(enum pFSM * , int);
int firstInstanceofDirective(enum pFSM * , int);
int firstInstanceofOpcode(enum pFSM * , int);
int firstInstanceofOperands(enum pFSM * , int);
/************************************************************************************************/



int main(int argc, char ** argv) {

    enum errorCode errorp;
    if (argc == 3) {
        FILE * infile = fopen(argv[1], "r");
        FILE * outfile = fopen(argv[2], "w");

        if (!infile) {
            //printf("Could not open input file \"%s\"", argv[1]);
            exit(4);
        } else if (!outfile) {
            //printf("Could not open output file \"%s\"", argv[2]);
            exit(4);
        } else {
            errorp = assemble(infile, outfile);

            fclose(infile);
            fclose(outfile);
        }
    } else {

        //printf("Input and Output file expected");
        exit(4);
    }

    exit(errorp);
}

enum errorCode assemble(FILE * in, FILE * out ) {
    symbol * symbolTable = malloc(sizeof(struct symbol));
    instruction * instructionTable = malloc(sizeof(struct instruction));

    int tableCount = 0;
    int tableCount2 = 0;
    enum errorCode errorp;

    errorp = createSymbolTable( in , START, & symbolTable, & tableCount, & instructionTable, & tableCount2);

    if (errorp == OK_VALID) {
        #ifdef Debug_Print_SymbolTable

        printf("\n\n\n");
        char * header = "/*******************Symbol Table*************************/";
        char * ender = "/********************************************************/";

        printWithIndent(header, 40);
        printf("\n");
        for (int j = 0; j < tableCount; j++) {
            printWithIndent("*", 40);
            char * addr = dec2hex(symbolTable[j].addr);
            printf("\t%-10s\t|\t%d(0x%s)\t\t*", symbolTable[j].symbolName, symbolTable[j].addr, addr);
            printf("\n");
            free(addr);
        }
        printWithIndent(ender, 40);
        printf("\n");
        #endif

        #ifdef Debug_Print_InstructionTable

        printf("\n\n\n");
        char * header1 = "/****************************************************Instruction Table*****************************************************************/";
        char * ender1 = "/**************************************************************************************************************************************/";

        printWithIndent(header1, 10);
        printf("\n");
        printWithIndent("*", 10);
        printf("\t%-10s\t|\t%-10s\t|\t%-10s\t|\t%-10s\t|\t%-10s\t\t%10s", "ADDRESS", "LABEL", "Directive", "Opcode", "Operands", "*");
        printf("\n");
        for (int j = 0; j < tableCount2; j++) {
            printWithIndent("*", 10);
            char * hex = dec2hex(instructionTable[j].addr);
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
            free(hex);
        }
        printWithIndent(ender1, 10);
        printf("\n");
        #endif

        for (int j = 0; j < tableCount2; j++) {

            for (int i = 4; i < 35; i++) {
                if (instructionTable[j].directive != NULL) {
                    if (strncmp(instructionTable[j].directive, inval[i], strlen(inval[i])) == 0 &&
                        strlen(instructionTable[j].directive) == strlen(inval[i])) {
                        errorp = encoder[i - 4]( & (instructionTable[j]), symbolTable, tableCount);
                    }
                } else if (instructionTable[j].opcode != NULL) {
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
            } else
                break;

        }
    }


    freeSymbolTable( & symbolTable, & tableCount);
    freeInstructionTable( & instructionTable, & tableCount2);

    return errorp;
}

void printWithIndent(char * text, int indent) {
    printf("%*s%s", indent, "", text);
}

void toUpperCase(char * str) {
    for (int i = 0; i < strlen(str); i++) {
        if (isalpha(str[i]) != 0)
            str[i] = toupper(str[i]);
    }
}

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

                        } else {
                            state = STOP;
                        }
                    } else {
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
                        } else if (stateT[0] == LABEL) {
                            insertSymbol(symbolTable, tableCount, tokens[0], index, startAddr);
                        } else if (stateT[0] == END || stateT[1] == END) {
                            state = PEND;
                        }
                        insertInstruction(instructionTable, tableCount2, & tokens, stateT, len1, index - 1, startAddr);
                    } else {
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

void insertSymbol(symbol ** symbolTable, int * tableCount, char * label, int line, char * startAddr) {
    if ( * tableCount > 0) {

        int j = 0;

        for (; j < * tableCount; j++) {
            if (strncmp(( * symbolTable)[j].symbolName, label, strlen(label)) == 0) {
                break;
            }
        }

        if (j == * tableCount) {
            * symbolTable = realloc( * symbolTable, (j + 1) * sizeof(struct symbol));
            ( * symbolTable)[j].symbolName = malloc(strlen(label) + 1);
            strncpy(( * symbolTable)[j].symbolName, label, strlen(label) + 1);
            ( * symbolTable)[j].addr = dec2dec2(startAddr) + (line - 1) * 2;
            * tableCount = ( * tableCount) + 1;
        }

    } else {
        ( * symbolTable)[0].symbolName = malloc(strlen(label) + 1);
        strncpy(( * symbolTable)[0].symbolName, label, strlen(label) + 1);
        ( * symbolTable)[0].addr = dec2dec2(startAddr) + (line - 1) * 2;
        * tableCount = ( * tableCount) + 1;
    }
}

void insertInstruction(instruction ** instructionTable, int * tableCount, char ** * tokens, enum pFSM * stateTransition, int step, int line, char * startAddr) {

    {
        ( * instructionTable) = realloc( * instructionTable, ( * tableCount + 1) * sizeof(struct instruction));
        ( * instructionTable)[ * tableCount].label = NULL;
        ( * instructionTable)[ * tableCount].directive = NULL;
        ( * instructionTable)[ * tableCount].opcode = NULL;
        ( * instructionTable)[ * tableCount].operands = NULL;
        ( * instructionTable)[ * tableCount].encoding = NULL;
        ( * instructionTable)[ * tableCount].opCount = 0;

        int labelIndex = firstInstanceofLabel(stateTransition, step);
        int directiveIndex = firstInstanceofDirective(stateTransition, step);
        int opcodeIndex = firstInstanceofOpcode(stateTransition, step);
        int operandIndex = firstInstanceofOperands(stateTransition, step);

        if (labelIndex == 0) {
            ( * instructionTable)[ * tableCount].label = malloc(strlen(( * tokens)[0]) + 1);
            strncpy(( * instructionTable)[ * tableCount].label, ( * tokens)[0], strlen(( * tokens)[0]) + 1);
        }

        if (directiveIndex < step) {
            ( * instructionTable)[ * tableCount].directive = malloc(strlen(( * tokens)[directiveIndex]) + 1);
            strncpy(( * instructionTable)[ * tableCount].directive, ( * tokens)[directiveIndex], strlen(( * tokens)[directiveIndex]) + 1);
        }

        if (opcodeIndex < step) {
            ( * instructionTable)[ * tableCount].opcode = malloc(strlen(( * tokens)[opcodeIndex]) + 1);
            strncpy(( * instructionTable)[ * tableCount].opcode, ( * tokens)[opcodeIndex], strlen(( * tokens)[opcodeIndex]) + 1);
        }

        if (operandIndex < step) {
            ( * instructionTable)[ * tableCount].operands = malloc((step - operandIndex) * sizeof(char ** ));

            for (int j = operandIndex; j < step; j++) {
                ( * instructionTable)[ * tableCount].operands[j - operandIndex] = malloc(strlen(( * tokens)[j]) + 1);
                strncpy(( * instructionTable)[ * tableCount].operands[j - operandIndex], ( * tokens)[j], strlen(( * tokens)[j]) + 1);
            }
            ( * instructionTable)[ * tableCount].opCount = step - operandIndex;
        }

        if (startAddr != NULL) {
            ( * instructionTable)[ * tableCount].addr = dec2dec2(startAddr) + line * 2;
        }

        * tableCount = * tableCount + 1;

    }
}

enum errorCode encodeORIG(instruction * instruct, symbol * symbol, int count) {
    if ((( * instruct).addr) % 2 != 0)
        return INVALID_CONSTANT;

    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return INVALID_CONSTANT;

    ( * instruct).encoding = dec2bin(( * instruct).addr);
    return OK_VALID;
}

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

enum errorCode encodeEND(instruction * instruct, symbol * symbol, int count) {
    return OK_VALID;
}

enum errorCode encodeNOP(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));
    strcat(str, "0000000000000000");
    str[16] = '\0';
    ( * instruct).encoding = str;

    return OK_VALID;
}

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

enum errorCode encodeHALT(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    strcat(str, "1111000000100101");
    str[16] = '\0';

    ( * instruct).encoding = str;

    return OK_VALID;
}

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

enum errorCode encodeRET(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    strcat(str, "1100000111000000");
    str[16] = '\0';

    ( * instruct).encoding = str;

    return OK_VALID;

}

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

enum errorCode encodeRTI(instruction * instruct, symbol * symbol, int count) {
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    strcat(str, "1000000000000000");
    str[16] = '\0';

    ( * instruct).encoding = str;

    return OK_VALID;

}

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

void freeLexemes(char ** * lexemes, int * len) {
    for (int i = 0; i < * len; i++) {
        free(( * lexemes)[i]);
    }

    free( * lexemes);
}

void freeSymbolTable(symbol ** symbolTable, int * tableCount) {

    for (int j = 0; j < * tableCount; j++) {
        free(( * symbolTable)[j].symbolName);
    }

    free( * symbolTable);
}

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
            strncpy(( * tokens)[c], token, strlen(token) + 1);
            toUpperCase(( * tokens)[c]);

            #ifdef Debug_Print
            //                  	printf("%s \n",token);
            #endif

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

char * dec2hex(int dec) {
    char * hex = malloc(sizeof(char * ));

    sprintf(hex, "%.4X", dec);

    return hex;
}

void prepend(char ** str, char * p) {
    char * str2 = calloc(strlen( * str) + strlen(p) + 1, sizeof(char));

    strcat(str2, p);
    strcat(str2, * str);
    str2[strlen( * str) + strlen(p)] = '\0';
    free( * str);
    * str = str2;
}

char * bin2hex(char * dec) {

    int dec2 = strtol(dec, NULL, 2);

    return dec2hex(dec2);
}

char * dec2bin(int dec) {
    char * dst = calloc(17, sizeof(char));

    dec = dec >= 0 ? dec : (65536 + dec);
    for (int i = 0; i <= 15; i++) {
        dst[15 - i] = ((dec >> i) & 1) ? '1' : '0';
    }

    dst[16] = '\0';

    return dst;
}

char * dec2dec(int dec) {
    char * dst = int2str(dec);
    prepend( & dst, "#");
    return dst;
}

int dec2dec2(char * dec) {
    if (isValidDec(dec))
        return strtol((dec + 1), NULL, 10);
    else
        return INT_MAX;
}

char * int2str(int dec) {
    char * str = malloc(sizeof(char * ));

    sprintf(str, "%d", dec);

    return str;
}

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

enum pFSM checkImmidiate(char ** str) {
    enum pFSM op = INVALID;

    if (strlen( * str) >= 2) {
        if (isValidHex( * str)) {
            int dec = hex2dec( * str);
            free( * str);
            * str = int2str(dec);
            prepend(str, "#");
            op = IMM;
        } else if (isValidDec( * str)) {
            op = IMM;
        }
    }

    return op;
}

enum errorCode integrityCheck(char ** * tokens, int * count, enum pFSM ** stateTransition) {

    int step = 0;
    bool flag;
    enum pFSM state = START;

    * stateTransition = malloc(sizeof(enum pFSM));

    while (step < * count) {
        * stateTransition = realloc( * stateTransition, (step + 1) * sizeof(enum pFSM));
        state = checkLabel(( * tokens)[step]);
        state = state == INVALID ? checkpOP(( * tokens)[step]) : state;
        state = state == INVALID ? checkInst(( * tokens)[step]) : state;
        state = state == INVALID ? checkRegister(( * tokens)[step]) : state;
        state = state == INVALID ? checkImmidiate( & (( * tokens)[step])) : state;

        ( * stateTransition)[step] = state;

	if(state == TRAP)
	{
		if(step < *count)
		{
			if(!isValidHex((*tokens)[step + 1]))
			{
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
            flag &= (( * stateTransition)[j] == pTransitions[i][j]);
        }

        if (flag) {

            flag &= (j < 5 ? (pTransitions[i][j] == IGN) : flag);
            if (flag)
                break;
        }
    }

    if (!flag) {
        int i = firstInstanceofOperands( * stateTransition, step);

        if (i == 0) {
            if (( * stateTransition)[0] != ORIG &&
                ( * stateTransition)[0] != FILL &&
                ( * stateTransition)[0] != END) {
                return INVALID_OPCODE;
            }
        } else {
            if (( * stateTransition)[i - 1] == LABEL)
                return INVALID_OPCODE;
        }
        return OTHER_ERROR;
    }

    #ifdef Debug_Print
    if (flag) printf("True\n");
    else printf("False\n");
    #endif

    return OK_VALID;
}

int firstInstanceofLabel(enum pFSM * states, int len) {
    int j = 0;

    for (; j < len; j++) {
        if (states[j] == LABEL) break;
    }

    return j;
}

int firstInstanceofDirective(enum pFSM * states, int len) {
    int j = 0;

    for (; j < len; j++) {
        if (states[j] == ORIG || states[j] == FILL || states[j] == END) break;
    }

    return j;

}

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

int firstInstanceofOperands(enum pFSM * states, int len) {
    int j = 0;

    for (; j < len; j++) {
        if (states[j] == REG || states[j] == IMM || (states[j] == LABEL && j > 0)) break;
    }

    return j;

}

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

bool checkValidRange(int dec, int numBytes) {
    return ((dec <= (pow(2, numBytes - 1) - 1)) && (dec >= -pow(2, numBytes - 1)));
}
