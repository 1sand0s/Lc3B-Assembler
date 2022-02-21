/*
 * Lc3b-Assembler.h
 *
 *  Created on: Feb 5, 2022
 *      Author: 1sand0s
 */

#ifndef LC3B_ASSEMBLER_H_
#define LC3B_ASSEMBLER_H_

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
    ORIG,   /**< ORIG */
    FILL,   /**< FILL */
    END,    /**< END */
    ADD,    /**< ADD */
    AND,    /**< AND */
    BR,     /**< BR */
    BRN,    /**< BRN */
    BRZ,    /**< BRZ */
    BRP,    /**< BRP */
    BRNZ,   /**< BRNZ */
    BRNP,   /**< BRNP */
    BRZP,   /**< BRZP */
    BRNZP,  /**< BRNZP */
    JMP,    /**< JMP */
    JSR,    /**< JSR */
    JSRR,   /**< JSRR */
    LDB,    /**< LDB */
    LDW,    /**< LDW */
    LEA,    /**< LEA */
    NOP,    /**< NOP */
    NOT,    /**< NOT */
    RET,    /**< RET */
    LSHF,   /**< LSHF */
    RSHFL,  /**< RSHFL */
    RSHFA,  /**< RSHFA */
    RTI,    /**< RTI */
    STB,    /**< STB */
    STW,    /**< STW */
    TRAP,   /**< TRAP */
    XOR,    /**< XOR */
    HALT,   /**< HALT */
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
enum pFSM checkLabel(char * );
enum pFSM checkpOP(char * );
enum pFSM checkInst(char * );
enum pFSM checkRegister(char * );
enum pFSM checkImmidiate(char ** );
enum errorCode integrityCheck(char ** * , int * , enum pFSM ** );
int matchLexemes(const enum pFSM *, const enum pFSM *, int);
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

#endif /* LC3B_ASSEMBLER_H_ */
