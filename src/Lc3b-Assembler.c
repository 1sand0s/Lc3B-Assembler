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
/*#define Debug_Print*/

/** +
 * @def Debug_Print_InstructionTable
 * @brief Comment to stop printing the Instruction table to stdout
 *
 */
#define Debug_Print_InstructionTable

/** +
 * @def Debug_Print_SymbolTable
 * @brief Comment to stop printing the Symbol table to stdout
v *
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
 * @brief Maximum length/number of lexemes allowed in a valid instruction
 *
 */
#define MAX_INSTRUCTION_LENGTH_ 5

/** +
 * @def MAX_VALID_COMBINATIONS_
 * @brief Number of valid combinations of tokens
 *
 */
#define MAX_VALID_COMBINATIONS_ 68

/** +
 * @def NUMBER_OF_INSTRUCTIONS_
 * @brief Number of instructions in the Lc3B ISA
 *
 */
#define NUMBER_OF_INSTRUCTIONS_ 28

/** +
 * @def NUMBER_OF_REGISTERS_
 * @brief Number of registers supported by ISA
 *
 */
#define NUMBER_OF_REGISTERS_ 8

/** +
 * @def NUMBER_OF_DIRECTIVES_
 * @brief Number of directives supported by ISA
 *
 */
#define NUMBER_OF_DIRECTIVES_ 3

/** +
 * @def ADDRESS_WIDTH_
 * @brief Size of each addressable location
 *
 */
#define ADDRESS_WIDTH_ 16

/** +
 * @def STRINGIFY_MACRO_
 * @brief Stringifies a macro value (adds "")
 *
 */
#define STRINGIFY_MACRO_(x) #x

/* List of all names that are invalid to use a labels in Lc3B assembly file */
const char * inval[43] = {
			  "IN",
			  "OUT",
			  "GETC",
			  "PUTS",
#define D_ENTRY(a, b) STRINGIFY_MACRO_(.a),
    DIRECTIVE_TABLE_
#undef D_ENTRY
#define I_ENTRY(a, b) STRINGIFY_MACRO_(a),
    INSTRUCTION_TABLE_
#undef I_ENTRY
			  "R0",
			  "R1",
			  "R2",
			  "R3",
			  "R4",
			  "R5",
			  "R6",
			  "R7"
};

/* Directives of Lc3B ISA as char* (used in converting char* tokens to pFSM lexemes) */
const char * directives[NUMBER_OF_DIRECTIVES_]={
#define D_ENTRY(a, b) STRINGIFY_MACRO_(.a),
    DIRECTIVE_TABLE_
#undef D_ENTRY
};

/* Valid pFSM members for all directives of Lc3B ISA */
const enum pFSM pDirectives[NUMBER_OF_DIRECTIVES_]={
#define D_ENTRY(a, b) a,
    DIRECTIVE_TABLE_
#undef D_ENTRY
};

/* Valid Registers that can be used with Lc3B instructions */
const char * regs[NUMBER_OF_REGISTERS_]={
					 "R0",
					 "R1",
					 "R2",
					 "R3",
					 "R4",
					 "R5",
					 "R6",
					 "R7"
};

/* Opcodes of Lc3B ISA as char* (used in converting char* tokens to pFSM lexemes) */
const char * opcodes[NUMBER_OF_INSTRUCTIONS_]={
#define I_ENTRY(a, b) STRINGIFY_MACRO_(a),
    INSTRUCTION_TABLE_
#undef I_ENTRY
};

/* Valid pFSM members for all opcodes of Lc3B ISA */
const enum pFSM pOpcodes[NUMBER_OF_INSTRUCTIONS_] = {
#define I_ENTRY(a, b) a,
    INSTRUCTION_TABLE_
#undef I_ENTRY
};


/* Valid state transistions for lexemes of an instruction */
const enum pFSM pTransitions[MAX_VALID_COMBINATIONS_][MAX_INSTRUCTION_LENGTH_] = {
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

/** +
 * @fn enum errorCode (*[])(instruction*, symbol*, int)
 * @brief Function pointers to encode instructions
 *
 * @param instruction*   Instruction to encode
 * @param symbol*        Symbol if any to replace with address
 * @param int
 * @return errorCode     Any errors during encoding process
 */
enum errorCode( * encoder[NUMBER_OF_DIRECTIVES_ + NUMBER_OF_INSTRUCTIONS_])(instruction * , symbol * , int) = {
#define D_ENTRY(a, b) b,
   DIRECTIVE_TABLE_
#undef D_ENTRY

#define I_ENTRY(a, b) b,
   INSTRUCTION_TABLE_
#undef I_ENTRY
};

/** +
 * @fn int main(int, char**)
 * @brief main method
 *
 *              Two arguements from command line expected
 *               1. argv[1] : Name of the file containing assembly code <*.asm>
 *               2. argv[2] : Name of the file to write assembled hex code to <*.hex>
 * 
 * @param argc  Number of commnadline arguements
 * @param argv  Name of commandline arguements
 * @return int  Exit code
 */
int main(int argc, char ** argv) {

  enum errorCode errorp;
  
  if (argc == 3) {
    
    /* Open *.asm for read-only */
    FILE * infile = fopen(argv[1], "r");

    /* Open *.hex for write-mode */
    FILE * outfile = fopen(argv[2], "w");
    
    if (!infile) {
      exit(OTHER_ERROR);
    }
    else if (!outfile) {
      exit(OTHER_ERROR);
    }
    else {
      /* Begin assembly if expected number of arguments is correct */
      errorp = assemble(infile, outfile);
      fclose(infile);
      fclose(outfile);
    }
  }
  else {
    
    /* Missing commandline arguements */
    exit(OTHER_ERROR);
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
#ifdef Debug_Print_SymbolTable
    
    printf("\n\n\n");
    char * header = "/*******************Symbol Table*************************/";
    char * ender = "/********************************************************/";
    
    printWithIndent(header, INDENT_SYMBOL_);
    printf("\n");
    for (int j = 0; j < tableCount; j++) {
      printWithIndent("*", INDENT_SYMBOL_);

      /* Convert base10 address to hex string */
      char * addr = Base10Number2Base16String(symbolTable[j].addr);
      printf("\t%-10s\t|\t%d(0x%s)\t\t*", symbolTable[j].symbolName, symbolTable[j].addr, addr);
      printf("\n");
      free(addr);
    }
    printWithIndent(ender, INDENT_SYMBOL_);
    printf("\n");
#endif
    
#ifdef Debug_Print_InstructionTable
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
      char * hex = Base10Number2Base16String(instructionTable[j].addr); //Convert base10 address to hex string
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
	  enc = Base2String2Base16String((instructionTable[j].encoding));
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
  freeSymbolTable( & symbolTable, & tableCount);
  freeInstructionTable( & instructionTable, & tableCount2);
  
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
void printWithIndent(const char * text,
		     const int indent) {
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
 *        and also an instruction table for all the instructions
 *
 * @param in                FILE* the assembly code file <*.asm>
 * @param state             State of the assembly process
 * @param symbolTable       Struct to hold the symbol table
 * @param tableCount        Number of elements/symbols in symbol table
 * @param instructionTable  Struct to hold instructions in the assembly code
 * @param tableCount2       Number of elements/instructions in instruction table
 * @return                  Error code if any errors are encountered
 */
enum errorCode createSymbolTable(FILE * in ,
				 enum FSM state,
				 symbol ** symbolTable,
				 int * tableCount,
				 instruction ** instructionTable,
				 int * tableCount2) {
  char * line = NULL;
  size_t len = 0;
  bool flag = true;
  enum errorCode errorp;
  char * startAddr=NULL;

  /* Keep iterating through the file line by line unless any line (instruction) has an error */
  while (flag) {
    switch (state) {
    case START:

      /* Read line from assembly file (in) into line */
      if (getline( & line, & len, in ) == -1)
	/* If EOF then stop assembly */
	state = STOP;
      else {

	/* Pointer to hold tokens extracted from a line in the <*.asm> file*/
	char ** tokens;
	int tokenCount = 0;

	/* Extract the tokens from line */
	lexer(line, & tokens, & tokenCount);

	/* If line empty or completely commented out then continue */
	if (tokenCount == 0)
	  continue;

	/* Pointer to hold lexemes to be converted from tokens */
	enum pFSM * lexemes;

	/* Convert tokens to lexemes and check if instruction is valid */
	errorp = integrityCheck( & tokens, & tokenCount, & lexemes);

	/* If instruction is valid */
	if (errorp == OK_VALID) {

	  /* The first line of any Lc3B <*.asm> file must be a .ORIG directive */
	  if (*tableCount2 == 0){

	    /* If first instruction not .ORIG then stop assembly */
	    if(lexemes[0] != ORIG)
	      state = STOP;
	    else{

	      /* The .ORIG directive defines the starting address in virtual memory where the rest of 
	       * the following instructions will be stored
	       */
	      startAddr = Base10Number2String(Base10String2Number(tokens[1]));
	    }
	  }
	  else{
	    
	    /* If another .ORIG is encountered then the <*.asm> file is invalid so stop assembly */
	    if (lexemes[0] == ORIG)
	      state = STOP;

	    /* If a LABEL is the first lexeme then add it to the symbol table */
	    else if (lexemes[0] == LABEL)
	      insertSymbol(symbolTable, tableCount, tokens[0], *tableCount2, startAddr);

	    /* End assembly if a .END directive is encountered */
	    else if (lexemes[0] == END || lexemes[1] == END) 
	      state = PEND;
	  }

	  /* Insert the instruction into the instructionTable to encode later on during second pass */
	  insertInstruction(instructionTable, tableCount2, & tokens, lexemes, tokenCount, *tableCount2 - 1, startAddr);
	}
 
	/* If instruction has any error then assembly file is invalid so stop assembly */
	else
	  state = STOP;

	/* Free the lexemes and the tokens */
	free(lexemes);
	freeTokens( & tokens, & tokenCount);
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

  /* Free memory allocated to store lines from input <*.asm> file */
  if (line)
    free(line);
 
  rewind( in );

  /* Return the errocode */
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
void insertSymbol(symbol ** symbolTable,
		  int * tableCount,
		  char * label,
		  int line,
		  char * startAddr) {
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
      strncpy(( * symbolTable)[j].symbolName, label, (strlen(label) + 1));
      
      /* Copy address(base10) into symbol table */
      ( * symbolTable)[j].addr = Base10String2Number(startAddr) + (line - 1) * 2;
      * tableCount = ( * tableCount) + 1;
    }
  }
  else {
    /* Allocate memory to hold the label name*/
    ( * symbolTable)[0].symbolName = malloc(strlen(label) + 1);
    
    /* Copy label into symbol table */
    strncpy(( * symbolTable)[0].symbolName, label, (strlen(label) + 1));
    
    /* Copy address(base10) into symbol table */
    ( * symbolTable)[0].addr = Base10String2Number(startAddr) + (line - 1) * 2;
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
void insertInstruction(instruction ** instructionTable,
		       int * tableCount,
		       char ** * tokens,
		       enum pFSM * stateTransition,
		       int step,
		       int line,
		       char * startAddr) {

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
    strncpy(( * instructionTable)[ * tableCount].label, ( * tokens)[0], (strlen(( * tokens)[0]) + 1));
  }
  
  /* Copy directive lexeme if it exists into instruction table */
  if (directiveIndex < step) {
    ( * instructionTable)[ * tableCount].directive = malloc(strlen(( * tokens)[directiveIndex]) + 1);
    strncpy(( * instructionTable)[ * tableCount].directive, ( * tokens)[directiveIndex], (strlen(( * tokens)[directiveIndex]) + 1));
  }
  
  /* Copy opcode lexeme if it exists into instruction table */
  if (opcodeIndex < step) {
    ( * instructionTable)[ * tableCount].opcode = malloc(strlen(( * tokens)[opcodeIndex]) + 1);
    strncpy(( * instructionTable)[ * tableCount].opcode, ( * tokens)[opcodeIndex], (strlen(( * tokens)[opcodeIndex]) + 1));
  }
  
  /* Copy operand lexemes if it exists into instruction table */
  if (operandIndex < step) {
    ( * instructionTable)[ * tableCount].operands = malloc((step - operandIndex) * sizeof(char * ));
    
    for (int j = operandIndex; j < step; j++) {
      ( * instructionTable)[ * tableCount].operands[j - operandIndex] = malloc(strlen(( * tokens)[j]) + 1);
      strncpy(( * instructionTable)[ * tableCount].operands[j - operandIndex], ( * tokens)[j], (strlen(( * tokens)[j]) + 1));
    }
    ( * instructionTable)[ * tableCount].opCount = step - operandIndex;
  }

  /* Copy instruction address into instruction table */
  if (startAddr != NULL) {
    if(line < 0)
      line = 0;
    ( * instructionTable)[ * tableCount].addr = Base10String2Number(startAddr) + line * 2;
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
enum errorCode encodeORIG(instruction * instruct,
			  const symbol * symbol,
			  const int count) {

  /* Check if the starting address is word aligned */
  if ((( * instruct).addr) % 2 != 0)
    return INVALID_CONSTANT;

  /* Check if starting address range is valid */
  if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
    return INVALID_CONSTANT;
  
  ( * instruct).encoding = Base10Number2Base2String(( * instruct).addr);
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
enum errorCode encodeFILL(instruction * instruct,
			  const symbol * symbol,
			  const int count) {

  /* Check if address where this directive appears is valid*/
  if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
    return OTHER_ERROR;

  /* Get the operand of .FILL directive */
  int constant = strtol((*instruct).operands[0] + 1, NULL, 10);

  /* Check if the operand range is valid */
  if(constant < -32768 || constant > 65535)
    return INVALID_CONSTANT;

  ( * instruct).encoding = Base10Number2Base2String(constant);
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
enum errorCode encodeEND(instruction * instruct,
			 const symbol * symbol,
			 const int count) {
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
enum errorCode encodeNOP(instruction * instruct,
			 const symbol * symbol,
			 const int count) {

  /* Check if address where this directive appears is valid*/
  if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
    return OTHER_ERROR;

  /* Every addressable location can contain 16 bits (ADDRESS_WIDTH_) 
   * and we add 1 for NULL terminating character
   */
  char * str = calloc(ADDRESS_WIDTH_ + 1, sizeof(char));

  /* Encoding of the NOP instruction */
  strcat(str, "0000000000000000");

  /* Add the NULL terminating character */
  str[16] = '\0';

  /* Store encoded string into instruct */
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
enum errorCode encodeTRAP(instruction * instruct,
			  const symbol * symbol,
			  const int count) {
  
  /* Get the operand of TRAP instruction */
  int operand = strtol(( * instruct).operands[0] + 1, NULL, 10);

  /* CHeck if operand is in valid range */
  if(operand <0 || operand > 255)
    return INVALID_CONSTANT;
  
  /* Every addressable location can contain 16 bits (ADDRESS_WIDTH_) 
   * and we add 1 for NULL terminating character
   */
  char * str = calloc(ADDRESS_WIDTH_ + 1, sizeof(char));

  char * dr = Base10Number2Base2String(operand);

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
enum errorCode encodeHALT(instruction * instruct,
			  const symbol * symbol,
			  const int count) {

  /* Check if address where this instruction appears is valid */
  if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
    return OTHER_ERROR;

  /* Every addressable location can contain 16 bits (ADDRESS_WIDTH_) 
   * and we add 1 for NULL terminating character
   */
  char * str = calloc(ADDRESS_WIDTH_ + 1, sizeof(char));

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
enum errorCode encodeADD(instruction * instruct,
			 const symbol * symbol,
			 const int count) {

  /* Check if address where this instruction appears is valid */
  if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
    return OTHER_ERROR;

  /* Every addressable location can contain 16 bits (ADDRESS_WIDTH_) 
   * and we add 1 for NULL terminating character
   */
  char * str = calloc(ADDRESS_WIDTH_ + 1, sizeof(char));

  /* Get the operands of the instruction */
  int operand1 = strtol(( * instruct).operands[0] + 1, NULL, 10);
  int operand2 = strtol(( * instruct).operands[1] + 1, NULL, 10);
  int operand3 = strtol(( * instruct).operands[2] + 1, NULL, 10);

  /* The first 2 operands are Registers for ADD */
  char * dr = Base10Number2Base2String(operand1);
  char * sr = Base10Number2Base2String(operand2);

  strcat(str, "0001");

  /* The last 3 bits of dr and sr give the register thats passed as operand to ADD */
  strcat(str, dr + 13);
  strcat(str, sr + 13);
  char * sr1 = NULL;
  
  if (checkImmidiate( & ( * instruct).operands[2]) == IMM) {
    strcat(str, "1");
    if (!checkValidRange(operand3, 5)) {
	free(dr);
	free(sr);
	free(str);
	return INVALID_CONSTANT;
      }
    sr1 = Base10Number2Base2String(operand3);
    strcat(str, sr1 + 11);
  }
  else {
    strcat(str, "000");
    sr1 = Base10Number2Base2String(operand3);
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
enum errorCode encodeXOR(instruction * instruct,
			 const symbol * symbol,
			 const int count) {

  /* Check if address where this instruction appears is valid */
  if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
    return OTHER_ERROR;
  
  /* Every addressable location can contain 16 bits (ADDRESS_WIDTH_) 
   * and we add 1 for NULL terminating character
   */
  char * str = calloc(ADDRESS_WIDTH_ + 1, sizeof(char));

  /* Get the operands of the instruction */
  int operand1 = strtol(( * instruct).operands[0] + 1, NULL, 10);
  int operand2 = strtol(( * instruct).operands[1] + 1, NULL, 10);
  int operand3 = strtol(( * instruct).operands[2] + 1, NULL, 10);

  /* The first 2 operands are Registers for ADD */
  char * dr = Base10Number2Base2String(operand1);
  char * sr = Base10Number2Base2String(operand2);

  strcat(str, "1001");
  strcat(str, dr + 13);
  strcat(str, sr + 13);
  char * sr1 = NULL;
  if (checkImmidiate( & ( * instruct).operands[2]) == IMM) {
    strcat(str, "1");
    if (!checkValidRange(operand3, 5)) {
      free(dr);
      free(sr);
      free(str);
      return INVALID_CONSTANT;
    }
    sr1 = Base10Number2Base2String(operand3);
    strcat(str, sr1 + 11);
  }
  else {
    strcat(str, "000");
    sr1 = Base10Number2Base2String(operand3);
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
enum errorCode encodeAND(instruction * instruct,
			 const symbol * symbol,
			 const int count) {

  /* Check if address where this instruction appears is valid */
  if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
    return OTHER_ERROR;
  
  /* Every addressable location can contain 16 bits (ADDRESS_WIDTH_) 
   * and we add 1 for NULL terminating character
   */
  char * str = calloc(ADDRESS_WIDTH_ + 1, sizeof(char));

  /* Get the operands of the instruction */
  int operand1 = strtol(( * instruct).operands[0] + 1, NULL, 10);
  int operand2 = strtol(( * instruct).operands[1] + 1, NULL, 10);
  int operand3 = strtol(( * instruct).operands[2] + 1, NULL, 10);

  /* The first 2 operands are Registers for ADD */
  char * dr = Base10Number2Base2String(operand1);
  char * sr = Base10Number2Base2String(operand2);

  strcat(str, "0101");
  strcat(str, dr + 13);
  strcat(str, sr + 13);
  
  char * sr1 = NULL;
  if (checkImmidiate( & ( * instruct).operands[2]) == IMM) {
    strcat(str, "1");
    if (!checkValidRange(operand3, 5)) {
      free(dr);
      free(sr);
      free(str);
      return INVALID_CONSTANT;
    }
    sr1 = Base10Number2Base2String(operand3);
    strcat(str, sr1 + 11);
  }
  else {
    strcat(str, "000");
    sr1 = Base10Number2Base2String(operand3);
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
enum errorCode encodeJMP(instruction * instruct,
			 const symbol * symbol,
			 const int count) {
  
  if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
    return OTHER_ERROR;
  
  char * str = calloc(17, sizeof(char));
  
  char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
  
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
enum errorCode encodeRET(instruction * instruct,
			 const symbol * symbol,
			 const int count) {
  
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
enum errorCode encodeJSR(instruction * instruct,
			 const symbol * sym,
			 const int count) {
  
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
        char * dr = Base10Number2Base2String(offset);
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
enum errorCode encodeBR(instruction * instruct,
			const symbol * sym,
			const int count) {
  
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

        char * dr = Base10Number2Base2String(offset);
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
enum errorCode encodeBRN(instruction * instruct,
			 const symbol * sym,
			 const int count) {
  
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

        char * dr = Base10Number2Base2String(offset);
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
enum errorCode encodeBRZ(instruction * instruct,
			 const symbol * sym,
			 const int count) {
  
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

        char * dr = Base10Number2Base2String(offset);
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
enum errorCode encodeBRP(instruction * instruct,
			 const symbol * sym,
			 const int count) {
  
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

        char * dr = Base10Number2Base2String(offset);
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
enum errorCode encodeBRNZ(instruction * instruct,
			  const symbol * sym,
			  const int count) {
  
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

        char * dr = Base10Number2Base2String(offset);
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
enum errorCode encodeBRNP(instruction * instruct,
			  const symbol * sym,
			  const int count) {
  
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

        char * dr = Base10Number2Base2String(offset);
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
enum errorCode encodeBRZP(instruction * instruct,
			  const symbol * sym,
			  const int count) {
  
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

        char * dr = Base10Number2Base2String(offset);
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
enum errorCode encodeBRNZP(instruction * instruct,
			   const symbol * sym,
			   const int count) {
  
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

        char * dr = Base10Number2Base2String(offset);
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
enum errorCode encodeJSRR(instruction * instruct,
			  const symbol * symbol,
			  const int count) {
  
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));

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
enum errorCode encodeLDB(instruction * instruct,
			 const symbol * symbol,
			 const int count) {
  
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = Base10Number2Base2String(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0010");
    strcat(str, dr + 13);
    strcat(str, sr + 13);

    if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 6)) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }
    char * sr1 = Base10Number2Base2String(strtol(( * instruct).operands[2] + 1, NULL, 10));
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
enum errorCode encodeLDW(instruction * instruct,
			 const symbol * symbol,
			 const int count) {
  
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = Base10Number2Base2String(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0110");
    strcat(str, dr + 13);
    strcat(str, sr + 13);

    if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 6)) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }
    char * sr1 = Base10Number2Base2String(strtol(( * instruct).operands[2] + 1, NULL, 10));
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
enum errorCode encodeLEA(instruction * instruct,
			 const symbol * sym,
			 const int count) {
  
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
        char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
        char * sr = Base10Number2Base2String(offset);
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
enum errorCode encodeNOT(instruction * instruct,
			 const symbol * symbol,
			 const int count) {
  
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = Base10Number2Base2String(strtol(( * instruct).operands[1] + 1, NULL, 10));

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
enum errorCode encodeRTI(instruction * instruct,
			 const symbol * symbol,
			 const int count) {
  
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
enum errorCode encodeLSHF(instruction * instruct,
			  const symbol * symbol,
			  const int count) {
  
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = Base10Number2Base2String(strtol(( * instruct).operands[1] + 1, NULL, 10));

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

    char * sr1 = Base10Number2Base2String(strtol(( * instruct).operands[2] + 1, NULL, 10));
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
enum errorCode encodeRSHFL(instruction * instruct,
			   const symbol * symbol,
			   const int count) {
  
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = Base10Number2Base2String(strtol(( * instruct).operands[1] + 1, NULL, 10));

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

    char * sr1 = Base10Number2Base2String(strtol(( * instruct).operands[2] + 1, NULL, 10));
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
enum errorCode encodeRSHFA(instruction * instruct,
			   const symbol * symbol,
			   const int count) {
  
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = Base10Number2Base2String(strtol(( * instruct).operands[1] + 1, NULL, 10));

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

    char * sr1 = Base10Number2Base2String(strtol(( * instruct).operands[2] + 1, NULL, 10));
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
enum errorCode encodeSTB(instruction * instruct,
			 const symbol * symbol,
			 const int count) {
  
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = Base10Number2Base2String(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0011");
    strcat(str, dr + 13);
    strcat(str, sr + 13);

    if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 6)) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }

    char * sr1 = Base10Number2Base2String(strtol(( * instruct).operands[2] + 1, NULL, 10));
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
enum errorCode encodeSTW(instruction * instruct,
			 const symbol * symbol,
			 const int count) {
  
    if (( * instruct).addr < 0 || ( * instruct).addr >= 65536)
        return OTHER_ERROR;

    char * str = calloc(17, sizeof(char));

    char * dr = Base10Number2Base2String(strtol(( * instruct).operands[0] + 1, NULL, 10));
    char * sr = Base10Number2Base2String(strtol(( * instruct).operands[1] + 1, NULL, 10));

    strcat(str, "0111");
    strcat(str, dr + 13);
    strcat(str, sr + 13);

    if (!checkValidRange(strtol(( * instruct).operands[2] + 1, NULL, 10), 6)) {
        free(dr);
        free(sr);
        free(str);
        return INVALID_CONSTANT;
    }

    char * sr1 = Base10Number2Base2String(strtol(( * instruct).operands[2] + 1, NULL, 10));
    strcat(str, sr1 + 10);
    str[16] = '\0';
    free(sr);
    free(dr);
    free(sr1);

    ( * instruct).encoding = str;

    return OK_VALID;

}

/** +
 * @fn void freeTokens(char***, int*)
 * @brief Free memory allocated to store tokens
 *
 * @param tokens
 * @param len
 * @return void
 */
void freeTokens(char ** * tokens,
		int * len) {
    for (int i = 0; i < * len; i++) {
        free(( * tokens)[i]);
    }
    free( * tokens);
}

/** +
 * @fn void freeSymbolTable(symbol**, int*)
 * @brief Free the symbol table
 *
 * @param symbolTable
 * @param tableCount
 * @return void
 */
void freeSymbolTable(symbol ** symbolTable,
		     int * tableCount) {
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
void freeInstructionTable(instruction ** instructionTable,
			  int * tableCount) {
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
void lexer(char * line,
	   char ** * tokens,
	   int * len) {

#ifdef Debug_Print
  printf("\n%s\n", line);
#endif

  /* Get index of beginning of comments if any */
  size_t index = strcspn(line, ";");
  
  /* Ignore lines which are completely comments*/
  if (index != 0)
    {
      char * subbuff = malloc(index + 1);
      
      /* Copy line from <*.asm> file into temporary buffer */
      strncpy(subbuff, &line[0], index);
      subbuff[index] = '\0';

#ifdef Debug_Print
      printf("%s\n", subbuff);
#endif
      
      /* Tokenize line with \n and \t as seperators */
      char * token = strtok(subbuff, " ,\n\t");
      
      /* Initialize len to 0 */
      int c = 0;
      
      while (token != NULL) {
	* tokens = c == 0 ? malloc(sizeof(char * )) : realloc( * tokens, (c + 1) * sizeof(char * ));
	( * tokens)[c] = malloc((strlen(token) + 1) * sizeof(char));
	
	/* Copy current token from line into tokens variable */
	strncpy(( * tokens)[c], token, (size_t)(strlen(token) + 1));
	
	/* Convert all tokens to uppercase for uniformity */
	toUpperCase(( * tokens)[c]);

	/* Get next token */
	token = strtok(NULL, " ,\n\t");
	c++;
      }
      
      /* Keep reference to number of extracted tokens */
      *len = c;
      
      /* Free buffer used to store line from <*.asm> file */
      free(subbuff);
    }
}

/** +
 * @fn int Base16String2Base10Number(char*)
 * @brief Convert Base16 string into Base10 number
 *
 * @param str  Base16 string
 * @return int The corresponding Base10 number 
 */
int Base16String2Base10Number(char * str) {
  /* INT_MAX used as default placeholder to indicate invalid hex string */
  int dec = INT_MAX;
      
  /* If valid then convert to equivalent Base10 number */
  dec = isValidBase16(str) ? strtol((str + 1), NULL, 16) : dec;
  return dec;
}

/** +
 * @fn char * Base10Number2Base16String(int)
 * @brief Convert a Base10 number into Base16 string
 *     
 *
 * @param dec    The Base10 number to convert to Base16 string
 * @return char* The Base16 string, caller must free the memory
 */
char * Base10Number2Base16String(int dec) {
  /* Allocate memory to store Base16 string
   *
   *      int dec = 10;
   *      Base16 string = X000A -> 5 bytes 
   */
  char * hex = calloc(ADDRESS_WIDTH_/4 + 1, sizeof(char));

  /* Convert Base10 to Base16 string*/
  sprintf(hex, "%.4X", dec);
  return hex;
}

/** +
 * @fn void prepend(char**, char*)
 * @brief This function is used to prepend '#' or 'X' to Base10
 *        or Base16 string to make them valid
 *
 * @param str   The Base10 or Base16 string
 * @param p     The descriptor to prepend <'#' or 'X'>
 */
void prepend(char ** str,
	     char * p) {
  /* Allocate enough memory to store str and the string p to be prepended*/
  char * str2 = calloc(strlen( * str) + strlen(p) + 1, sizeof(char));

  /* Prepend string p to str2 */  
  strcat(str2, p);

  /* Concatenate string str to str2 */
  strcat(str2, * str);
  str2[strlen( * str) + strlen(p)] = '\0';

  /* Free the old string */
  free( * str);

  /* Store newly formed string in the old string */
  * str = str2;
}

/** +
 * @fn char *  Base2String2Base16String(char*)
 * @brief Convert Base2 string into Base16 string
 *
 * @param strBase2  Base2 string
 * @return char*    Corresponding Base16 string
 */
char * Base2String2Base16String(char * strBase2) {
  /* Convert Base2 string to Base10 Number */
  int dec = strtol(strBase2, NULL, 2);

  /* Convert Base10 number to Base16 string */  
  return Base10Number2Base16String(dec);
}

/** +
 * @fn char * Base10Number2BaseString(int)
 * @brief Convert Base10 number into Base2 string
 *
 * @param dec    Base10 number
 * @return char* Corresponding Base2 string
 */
char * Base10Number2Base2String(int dec) {
  /* Allocate memory to store the Base2 string */
  char * dst = calloc(ADDRESS_WIDTH_ + 1, sizeof(char));

  /* Integer overflow if dec > 16 bytes */
  dec = dec >= 0 ? dec : (65536 + dec);

  /* Convert Base10 number to Base2 string */
  for (int i = 0; i < ADDRESS_WIDTH_; i++) {
    dst[ADDRESS_WIDTH_ - 1 - i] = ((dec >> i) & 1) ? '1' : '0';
  }

  /* Add NULL terminating character */
  dst[ADDRESS_WIDTH_] = '\0';
  return dst;
}

/** +
 * @fn char * Base10Number2String(int)
 * @brief Convert Base10 number into Base10 string
 *
 * @param dec    Base10 number
 * @return char* Corresponding Base10 string
 */
char * Base10Number2String(int dec) {
  int numDigits = 1;
  int dec2 = dec;

  /* Get the number of digits so we can allocate string with 
   * proper size 
   */
  while(dec2 != 0){
    dec2 /= 10;
    numDigits++;
  }

  /* Add 2 more for '#' and '\0' */
  char * str = malloc((numDigits + 2) * sizeof(char));

  /* Store Base10 number as Base10 string*/
  sprintf(str, "%d", dec);

  /* Peprend '#' to make Base10 string valid */
  prepend( & str, "#");
  return str;
}

/** +
 * @fn int Base10String2Number(char*)
 * @brief Convert Base10 string into Base10 number
 *
 * @param str  Base10 string
 * @return int corresponding Base10 number
 */
int Base10String2Number(char * str) {

  /* Check if Base10 string is a valid Base10 string*/
    if (isValidBase10(str))
      /* Pass dec + 1 since dec points to the desriptor '#' */
      return strtol((str + 1), NULL, 10);
    else
      /* INT_MAX used to indicate invalid input Base10 string */
      return INT_MAX;
}

/** +
 * @fn bool isValidBase16(char*)
 * @brief Check if Base16/hex string is a valid Base16 number
 *       
 * @param str   Base16 string whose validity is to be confirmed
 * @return bool The validity of the Base16 string
 */
bool isValidBase16(char * str) {
   /* Flag to store validity of the input string */
  bool valid = false;
  int strLength = strlen(str);

  /* All valid Base16 strings are represented as follows
   *
   *    Descriptor |  Value
   *        X      |  3000
   *        X      |  -3050
   * 
   * Therefore all Base16 strings must have atleast 2 characters ("X" and the value)
   */
  if (strLength >= 2) {
    valid = true;

    /* Iterate through characters to make sure hex string is valid */
    for(int j = 0; j < strLength; j++)
      
      switch(j){
      case 0:
	/* The 0th index/first character of a valid Base16 string must contain 'X' */
	valid &= (str[j] == 'X');
	break;
	
      case 1:
	/* The 1st index/second character can either be a hex digit or sign <+/-> */
	valid &= (str[j] == '+' || str[j] == '-' || (isxdigit(str[j]) != 0));
	break;
	  
      default :
	/* The remaining indices must all be hex digits [0-9, A-F] for a valid Base16 string */
	valid &= (isxdigit(str[j]) != 0);
	break;
      }
  }
  return valid;
}

/** +
 * @fn bool isValidBase10(char*)
 * @brief Check if a Base10 string is a valid Base10 number
 *
 * @param str   The Base10 string to check for validity
 * @return bool The validity of the input string str
 */
bool isValidBase10(char * str) {
  /* Flag to store validity of the input string */
  bool valid = false;
  int strLength = strlen(str);

  /* All valid Base10 strings are represented as follows
   *
   *    Descriptor |  Value
   *        #      |  10
   *        #      |  -1
   * 
   * Therefore all Base10 strings must have atleast 2 characters ("#" and the value)
   */
  if(strLength >= 2){
    valid = true;
    
    /* Iterate through characters of str to check validity */
    for(int j = 0; j < strLength; j++)
      
      switch(j){
      case 0:
	/* The 0th index/first character of a valid Base10 string must contain '#' */
	valid &= (str[j] == '#');
	break;
	
      case 1:
	/* The 1st index/second character can either be a digit or sign <+/-> */
	valid &= (str[j] == '+' || str[j] == '-' || (isdigit(str[j]) != 0));
	break;
	
      default :
	/* The remaining indices must all be digits [0-9] for a valid Base10 string */
	valid &= (isdigit(str[j]) != 0);
	break;
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
enum pFSM checkLabel(const char * str) {
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
 * @fn enum pFSM checkDirective(char*)
 * @brief Check if directive is valid
 *
 * @param str   string containing directive
 * @return pFSM lexeme (ORIG, FILL, END) or INVALID
 */
enum pFSM checkDirective(const char * str) {
  enum pFSM op = INVALID;
  if (strlen(str) > 0) 

    /* Iterate through valid directive names given in directives variable
     * to find a match
     */
    for (int j = 0; j < NUMBER_OF_DIRECTIVES_; j++)
      if (strncmp(str, directives[j], strlen(str)) == 0)
	if (strlen(str) == strlen(directives[j])) {
	  op = pDirectives[j];
	  break;
	}
  return op;
}

/** +
 * @fn enum pFSM checkInst(char*)
 * @brief Check if opcode used exists in ISA
 *
 * @param str   string containing opcode
 * @return pFSM lexeme (instruction) or INVALID
 */
enum pFSM checkInst(const char * str) {
  enum pFSM op = INVALID;
  if (strlen(str) > 0)
    
    /* Iterate through valid opcode names given in opcodes variable
     * to find a match
     */
    for (int j = 0; j < NUMBER_OF_INSTRUCTIONS_; j++)
      if (strncmp(str, opcodes[j], strlen(str)) == 0)
	if (strlen(str) == strlen(opcodes[j])) {
	  op = pOpcodes[j];
	  break;
	}
  return op;
}

/** +
 * @fn enum pFSM checkRegister(char*)
 * @brief Check if registers used are valid (R[0-7])
 *
 * @param str   string containing register name
 * @return pFSM lexeme (REG) or INVALID
 */
enum pFSM checkRegister(const char * str) {
  enum pFSM op = INVALID;

  /* A register name must contain exactly two characters
   * [R][0-7]
   */
  if (strlen(str) == 2)

    /* Iterate through valid register names given in regs variable
     * to find a match
     */
    for (int j = 0; j < NUMBER_OF_REGISTERS_; j++)
      if (strncmp(str, regs[j], strlen(str)) == 0)
	if (strlen(str) == strlen(regs[j])) {
	  op = REG;
	  break;
	}
  return op;
}

/** +
 * @fn enum pFSM checkImmidiate(char**)
 * @brief Check if immediate values used are valid and convert to 
 *        Base10 number for uniformity
 *
 * @param str   Immediate value 
 * @return pFSM lexeme (IMM) or INVALID
 */
enum pFSM checkImmidiate(char ** str) {
  /* Default placeholder if immediate is invalid*/
  enum pFSM op = INVALID;

  /* All immediate operands must contain a descriptor indicating their base
   * hence must be atleast 2 characters (['X'/'#'],[value])
   */
  if (strlen( * str) >= 2) {

    /* If immediate is a hex string then convert to Base10 string */
    if (isValidBase16( * str)) {

      /* Get Base10 number of Base16 string*/
      int dec = Base16String2Base10Number( * str);

      /* Free Base16 string */
      free( * str);

      /* Convert Base10 number to valid Base16 Number */
      * str = Base10Number2String(dec);
      op = IMM;
    }
    else if (isValidBase10( * str))
      op = IMM;
  }
  return op;
}

/** +
 * @fn void convertTokens(char***, int*, enum pFSM**, int*)
 * @brief Converts tokens into corresponding lexemes
 *
 *        Example :
 *        Instruction :   A      LEA    R1,     B
 *        Tokens      : {"A",   "LEA", "R1",   "B"}
 *        Lexeme Array: {LABEL,  LEA,   REG,  LABEL} 
 *
 *       
 *
 * @param tokens           Tokens after parsing a line containing an instruction
 * @param count            Number of tokens
 * @param stateTransition  Corresponding lexeme array (must be freed by the caller)
 */
void convertTokens(char ** * tokens,
		   int * count,
		   enum pFSM ** stateTransition) {
  int step = 0;
  enum pFSM state = START;

  /* Allocate memory to lexeme array */
  *stateTransition = calloc(*count, sizeof(enum pFSM));

  /* Iterate through tokens and convert to corresponding lexemes */
  while (step < *count) {

    /* Check if current token is a LABEL */
    state = checkLabel(*((*tokens) + step));

    /* Check if current token is a Directive (ORIG, FILL, END) */
    state = state == INVALID ? checkDirective(*((*tokens) + step)) : state;

    /* Check if current token is an opcode (any lexeme in pOpcodes) */
    state = state == INVALID ? checkInst(*((*tokens) + step)) : state;

    /* Check if current token is a register operand (REG) */
    state = state == INVALID ? checkRegister(*((*tokens) + step)) : state;

    /* Check if current token is an immediate operand (IMM) */
    state = state == INVALID ? checkImmidiate(((*tokens) + step)) : state;

    /* Assign state to current index of lexeme array */
    *((*stateTransition) + step) = state;
    step++;
  }
}

/** +
 * @fn enum errorCode integrityCheck(char***, int*, enum pFSM**)
 * @brief Checks the integerity of an instruction by converting its tokens
 *        into corresponding lexemes and comparing them with list of valid
 *	  lexeme sequences (pTransitions)
 *
 *        Example :
 *        Instruction :   A      LEA    R1,     B
 *        Tokens      : {"A",   "LEA", "R1",   "B"}
 *        Lexeme Array: {LABEL,  LEA,   REG,  LABEL} 
 *
 *        If Lexeme Array matches any pattern in pTransitions then return OK_VALID
 *        Else return appropriate error
 *       
 *
 * @param tokens           Tokens after parsing a line containing an instruction
 * @param count            Number of tokens
 * @param stateTransition  Corresponding lexeme array (must be freed by the caller)
 * @return errorCode       Error if the instruction is invalid
 */
enum errorCode integrityCheck(char ** * tokens,
			      int * count,
			      enum pFSM ** stateTransition) {

  /* Convert tokens(stored as char*) into lexemes(stored as enum pFSM) */
  convertTokens(tokens, count, stateTransition);
  
  /* Check if lexeme array matches any pattern in pTransitions (list of valid lexeme sequences
   * for all instructions in the Lc3b ISA)
   */
  for (int i = 0; i < MAX_VALID_COMBINATIONS_; i++)

    /* Match lexeme array with valid patterns */
    if(matchLexemes(pTransitions[i], *stateTransition, *count) == *count){

      /* If the two arrays agree with each other up to *count then match may still be partial 
       *
       *        Example : 
       *        Instruction             :    A    ADD   R1, R1
       *        Lexeme Array            : {LABEL, ADD, REG, REG}
       *        Partial Matching Pattern: {LABEL, ADD, REG, REG, REG/IMM}
       *                                                           ^
       *                                                           |
       *                                               Missing operand so still invalid
       *                                               despite partial match
       *
       */
      if(*count < MAX_INSTRUCTION_LENGTH_){

	/* Check if the lexeme following the last matched index is IGN (Ignorable) */
	if(pTransitions[i][*count] == IGN)
	  return OK_VALID;
      }

      /* If index of mismatch between the two arrays is equal to MAX_INSTRUCTION_LENGTH_ then 
       * the lexeme array must correspond to a valid instruction so return valid
       */
      else
	return OK_VALID;
    }

  /* If lexeme array not matched in valid patterns then find type of error */
  /* Get index in lexeme array where operands first appear */
  int i = firstInstanceofOperands(*stateTransition,
				  *count);

  /* If operands appear at 0th index then return INVALID_OPCODE since
   * operands must follow opcode/directive
   */
  if (i == 0) 
    return INVALID_OPCODE;
  
  /* If label appears immediately before operands then return INVALID_OPCODE since
   * labels can only appear at the beginning of an instruction or at the end
   */
  else if ((*stateTransition)[i - 1] == LABEL)
    return INVALID_OPCODE;
  else
    return OTHER_ERROR;
}

/** +
 * @fn int matchLexemes(enum pFSM*, enum pFSM*, int)
 * @brief Gives the index of the first mismatch between two lexeme arrays
 *
 *     Example :   
 *     Lexeme array1 : {LABEL, LEA, REG, LABEL, IGN}
 *     Lexeme array2 : {LABEL, ADD, REG, REG,   IMM}
 *                              ^
 *                              |
 *                  Index of first mismatch (index = 1)
 *
 * @param lArray1 Lexeme array 1
 * @param lArray2 Lexeme array 2
 * @param count   Length of the lexeme arrays (the arrays must be of same length)
 * @return int    Index of first mismatch (if equal to count then the two arrays are identical 
 *                w.r.t to lexemes)
 */
int matchLexemes(const enum pFSM * lArray1,
		 const enum pFSM * lArray2,
		 const int count){
  /* Index of mismatch , should be count if no mismatch and lexeme arrays are equal */
  int index = -1;

  /* Iterate and compare the lexeme arrays */
  while((index++ < (count-1)) && (*lArray1++ == *lArray2++));

  return index;
}

/** +
 * @fn int firstInstanceofLabel(enum pFSM*, int)
 * @brief Gives the index of label in the lexeme array of an instruction
 *
 *     Example : 
 *     Instruction  :    B    LEA  R1,    A  
 *     Lexeme array : {LABEL, LEA, REG, LABEL, IGN}
 *                       ^
 *                       |
 *              First Index of Label (j = 0)
 *
 * @param states Lexeme array
 * @param len    Length of lexeme array
 * @return int   Index of label in lexeme array (if equal to len then not found in given lexeme array)
 */
int firstInstanceofLabel(enum pFSM * states,
			 int len) {
  int j = 0;
  for (; j < len; j++) {
    
    /* The only valid pFSM state for label lexeme is LABEL */
    if (states[j] == LABEL) break;
  }
  return j;
}

/** +
 * @fn int firstInstanceofDirective(enum pFSM*, int)
 * @brief Gives the index of directive in the lexeme array of an instruction
 *
 *     Example : 
 *     Instruction  :    A    FILL  x3000
 *     Lexeme array : {LABEL, FILL,  IMM, IGN, IGN}
 *                             ^
 *                             |
 *                   First Index of Directive (j = 1)
 *
 * @param states Lexeme Array
 * @param len    Length of lexeme array
 * @return int   Index of directives in lexeme array (if equal to len then not found in given lexeme array)
 */
int firstInstanceofDirective(enum pFSM * states,
			     int len) {
  int j = 0;
  for (; j < len; j++) {

    /* The only allowed pFSM states that can be used as directives are ORIG, FILL and END */
    if (states[j] == ORIG || states[j] == FILL || states[j] == END) break;
  }
  return j;
}

/** +
 * @fn int firstInstanceofOpcode(enum pFSM*, int)
 * @brief Gives the index of opcode in the lexeme array of an instruction
 *
 *     Example : 
 *     Instruction  :    A    ADD  R3,  R3,  #-1
 *     Lexeme array : {LABEL, ADD, REG, REG, IMM}
 *                             ^
 *                             |
 *                   First Index of Opcode (j = 1)
 *
 * @param states Lexeme Array
 * @param len    Length of lexeme array
 * @return int   Index of opcode in lexeme array (if equal to len then not found in given lexeme array)
 */
int firstInstanceofOpcode(enum pFSM * states,
			  int len) {
    int j = 0;
    for (; j < len; j++) {
        int i = 0;

	/* Iterate through pOpcodes (stores lexemes (pFSM states) of all instructions)
	 * to check if there is a match for any lexeme in the given lexeme array 
	 */
        for (; i < NUMBER_OF_INSTRUCTIONS_ ; i++)
            if (states[j] == pOpcodes[i]) break;

	/* If found than exit loop */
        if (i != NUMBER_OF_INSTRUCTIONS_) break;
    }
    return j;
}

/** +
 * @fn int firstInstanceofOperands(enum pFSM*, int)
 * @brief Gives the index of operands in the lexeme array of an instruction
 *
 *     Example : 
 *     Instruction  :    A    ADD  R3,  R3,  #-1
 *     Lexeme array : {LABEL, ADD, REG, REG, IMM}
 *                                  ^
 *                                  |
 *                   First Index of Operands (j = 2)
 *                      
 * @param states Lexeme array
 * @param len    Length of lexeme array
 * @return int   Index of operands in lexeme array (if equal to len then not found in given lexeme array)
 */
int firstInstanceofOperands(enum pFSM * states,
			    int len) {
  int j = 0;
  for (; j < len; j++) {

    /* pFSM states REG, IMM and LABELS are the only valid lexemes that can be 
     * used as operands 
     */
    if (states[j] == REG || states[j] == IMM || (states[j] == LABEL && j > 0))
      break;
  }
  return j;
}

/** +
 * @fn int getAddrofLabel(symbol*, int, char*)
 * @brief Get address of label by searching the symbol table
 *
 * @param sym   The symbol table
 * @param count Number of entries in the symbol table
 * @param label Label whose address is to be found
 * @return int  The address of the label (-1 if not found)
 */
int getAddrofLabel(const symbol * sym,
		   const int count,
		   const char * label) {
  /* Use -1 as default placeholder to indicate missing label in symbol table */
  int addr = -1;

  /* Iterate through all symbol table entries to find if label exists */
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
 * @brief Checks if a signed Base10 number (dec) is representable by 
 *        specified number of bits (numBits)
 *
 * @param dec      Base10 number
 * @param numBits Specified number of bits
 * @return bool    
 */
bool checkValidRange(const int dec,
		     const int numBits) {
  /* A signed 5-bit Base10 number can only exist in the closed interval [-2^4, 2^4 - 1] */
  return ((dec <= (pow(2, numBits - 1) - 1)) && (dec >= -pow(2, numBits - 1)));
}

