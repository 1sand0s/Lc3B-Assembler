/*
 * Lc3b-Assembler.h
 *
 *  Created on: Feb 5, 2022
 *      Author: 1sand0s
 */

#ifndef LC3B_ASSEMBLER_H_
#define LC3B_ASSEMBLER_H_

#define DIRECTIVE_TABLE_\
  D_ENTRY(ORIG ,encodeORIG )\
  D_ENTRY(FILL ,encodeFILL )\
  D_ENTRY(END  ,encodeEND  )\

#define INSTRUCTION_TABLE_\
  I_ENTRY(ADD  ,encodeADD  )\
  I_ENTRY(AND  ,encodeAND  )\
  I_ENTRY(JMP  ,encodeJMP  )\
  I_ENTRY(JSR  ,encodeJSR  )\
  I_ENTRY(JSRR ,encodeJSRR )\
  I_ENTRY(LDB  ,encodeLDB  )\
  I_ENTRY(LDW  ,encodeLDW  )\
  I_ENTRY(LEA  ,encodeLEA  )\
  I_ENTRY(NOP  ,encodeNOP  )\
  I_ENTRY(NOT  ,encodeNOT  )\
  I_ENTRY(RET  ,encodeRET  )\
  I_ENTRY(LSHF ,encodeLSHF )\
  I_ENTRY(RSHFL,encodeRSHFL)\
  I_ENTRY(RSHFA,encodeRSHFA)\
  I_ENTRY(RTI  ,encodeRTI  )\
  I_ENTRY(STB  ,encodeSTB  )\
  I_ENTRY(STW  ,encodeSTW  )\
  I_ENTRY(TRAP ,encodeTRAP )\
  I_ENTRY(XOR  ,encodeXOR  )\
  I_ENTRY(HALT ,encodeHALT )\
  I_ENTRY(BR   ,encodeBR   )\
  I_ENTRY(BRN  ,encodeBRN  )\
  I_ENTRY(BRZ  ,encodeBRZ  )\
  I_ENTRY(BRP  ,encodeBRP  )\
  I_ENTRY(BRNZ ,encodeBRNZ )\
  I_ENTRY(BRNP ,encodeBRNP )\
  I_ENTRY(BRZP ,encodeBRZP )\
  I_ENTRY(BRNZP,encodeBRNZP)\

/** +
 * @enum FSM
 * @brief Enum to define assembly process states
 *
 */
enum FSM {
    START, /**< START : Beginning of assembly */
    PSTART,/**< PSTART : Begnning of code (ORIG directive valid) */
    PEND,  /**< PEND : End of code (END directive valid) */
    STOP   /**< STOP : End of assembly */
};

/** +
 * @enum pFSM
 * @brief Enum to define states during encoding of instructions
 *
 */
enum pFSM {
    LABEL,  /**< LABEL */
#define D_ENTRY(a, b) a,
    DIRECTIVE_TABLE_
#undef D_ENTRY

#define I_ENTRY(a, b) a,
    INSTRUCTION_TABLE_
#undef I_ENTRY
    INVALID,/**< INVALID */
    REG,    /**< REG */
    IMM,    /**< IMM */
    IGN     /**< IGN */
};

/** +
 * @enum errorCode
 * @brief Enum to define possible error codes during assembly
 *
 */
enum errorCode {
    OK_VALID,        /**< OK_VALID : No Error */
    UNDEFINED_LABEL, /**< UNDEFINED_LABEL : LABEL used without definition */
    INVALID_OPCODE,  /**< INVALID_OPCODE : OPCODE doesn't exist in ISA */
    INVALID_CONSTANT,/**< INVALID_CONSTANT : Illegal use of CONSTANT operand */
    OTHER_ERROR      /**< OTHER_ERROR : All other errors */
};

/** +
 * @struct symbol
 * @brief Struct to hold symbols during parsing
 *
 */
typedef struct symbol {
  char * symbolName; /* Name of the symbol */
  int addr;          /* address where the symbol is first encountered */
}
symbol;

/** +
 * @struct instruction
 * @brief Struct to hold instruction information
 *
 *          example 1:  ADDRESS | LABEL | DIRECTIVE | OPERANDS
 *                       x3000      A      .FILL       x3050
 *                      
 *                      char *label     = "A"
 *                      char *directive = ".FILL"
 *                      char *opcode    = NULL
 *                      char *operands  = {"#12368"}
 *                      char *encoding  = "0011000001010000"
 *                      int addr        = 12288
 *                      int opCount     = 1 
 *
 *
 *          example 2:  ADDRESS | LABEL | DIRECTIVE |    OPERANDS
 *                       x3000               ADD        R3, R3, #1
 *                      
 *                      char *label     = NULL
 *                      char *directive = NULL
 *                      char *opcode    = "ADD"
 *                      char *operands  = {"R3", "R3", "#1"}
 *                      char *encoding  = "0001011011100001"
 *                      int addr        = 12288
 *                      int opCount     = 3 
 *
 *
 */
typedef struct instruction {
  char * label;       /* Label if any */
  char * directive;   /* Directive if any (Directive and Opcode mututally exclusive) */
  char * opcode;      /* Opcode if any */
  char ** operands;   /* List of Operands (Registers, Constants etc) */
  char * encoding;    /* Instruction encoding after assembly */
  int addr;           /* Address of the instruction */
  int opCount;        /* Number of Operands */
}
instruction;


/***************************************Lexeme Integrity Checker*********************************/
enum pFSM checkLabel(const char * );
enum pFSM checkpOP(const char * );
enum pFSM checkInst(const char * );
enum pFSM checkRegister(const char * );
enum pFSM checkImmidiate(char ** );
enum errorCode integrityCheck(char ** * , int * , enum pFSM ** );
int matchLexemes(const enum pFSM *, const enum pFSM *, const int);
/************************************************************************************************/



/***************************************Find Lexemes in StateTransitionTable*********************/
int firstInstanceofLabel(enum pFSM * , int);
int firstInstanceofDirective(enum pFSM * , int);
int firstInstanceofOpcode(enum pFSM * , int);
int firstInstanceofOperands(enum pFSM * , int);
/************************************************************************************************/


/********************************Lexing Methods******************************************/
enum errorCode assemble(FILE *, FILE* );
void lexer(char * , char ** * , int * );
enum errorCode createSymbolTable(FILE * , enum FSM, symbol ** , int * , instruction ** , int * );
void insertSymbol(symbol ** , int * , char * , int, char * );
void insertInstruction(instruction ** , int * , char ** * , enum pFSM * , int, int, char * );
/****************************************************************************************/



/********************************Data Conversion Methods*********************************/
int Base16String2Base10Number(char * );
char * Base10Number2Base16String(int);
char * Base10Number2Base2String(int);
char * Base2String2Base16String(char * );
bool isValidBase16(char * );
bool isValidBase10(char * );
char * Base10Number2String(int);
int Base10String2Number(char * );
bool checkValidRange(const int, const int);
/***************************************************************************************/



/******************************Cleanup Methods******************************************/
void freeTokens(char ** * , int * );
void freeSymbolTable(symbol ** , int * );
void freeInstructionTable(instruction ** , int * );
/***************************************************************************************/



/******************************Miscellaneous Methods************************************/
void printWithIndent(const char * , const int);
void prepend(char ** , char * );
int getAddrofLabel(const symbol * , const int, const char * );
void toUpperCase(char * );
/***************************************************************************************/



/***************************************Encoder Methods*********************************/
#define D_ENTRY(a, b) enum errorCode b(instruction * , const symbol * , const int);
    DIRECTIVE_TABLE_
#undef D_ENTRY

#define I_ENTRY(a, b) enum errorCode b(instruction * , const symbol * , const int);
    INSTRUCTION_TABLE_
#undef I_ENTRY
/***************************************************************************************/

#endif /* LC3B_ASSEMBLER_H_ */
