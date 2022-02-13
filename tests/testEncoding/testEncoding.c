/*
 * testAssembler.c
 *
 *  Created on: Feb 6, 2022
 *      Author: 1sand0s
 */

#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include "../../src/Lc3b-Assembler.h"

/** +
 * @fn  START_TEST()
 * @brief Test case for ORIG instruction
 *
 * @param testEncoding_ORIG_CORRECT__
 */
START_TEST (testEncoding_ORIG_CORRECT_)
{
    instruction orig;
    enum errorCode error;

    /* Intialize struct instruct */
    orig.label = NULL;
    orig.directive = "ORIG";
    orig.opcode = NULL;
    orig.operands = NULL;
    orig.addr = 12288;//0x3000
    orig.opCount = 1;

    /* Encode orig instruct */
    error = encodeORIG(&orig, NULL, 0);

    /* Assert exit code as OK_VALID */
    ck_assert(error == OK_VALID);
    ck_assert_str_eq(orig.encoding, "0011000000000000");
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for ORIG instruction
 *
 * @param testEncoding_ORIG_INCORRECT_LESS_THAN_0_
 */
START_TEST (testEncoding_ORIG_INCORRECT_LESS_THAN_0_)
{
    instruction orig;
    enum errorCode error;

    /* Initialize struct instruct */
    orig.label = NULL;
    orig.directive = "ORIG";
    orig.opcode = NULL;
    orig.operands = NULL;
    orig.addr = -1;//Negative address
    orig.opCount = 1;

    /* Encode orig instruct */
    error = encodeORIG(&orig, NULL, 0);

    /* Assert exit code as INVALID_CONSTANT
     * Exit code not 0 since address is invalid (<0)
     */
    ck_assert(error == INVALID_CONSTANT);
}
END_TEST


/** +
 * @fn  START_TEST()
 * @brief Test case for ORIG instruction
 *
 * @param testEncoding_ORIG_INCORRECT_GREATER_THAN_65535_
 */
START_TEST (testEncoding_ORIG_INCORRECT_GREATER_THAN_65535_)
{
    instruction orig;
    enum errorCode error;

    /* Initialize struct instruct */
    orig.label = NULL;
    orig.directive = "ORIG";
    orig.opcode = NULL;
    orig.operands = NULL;
    orig.addr = 65536;//Address greater than 65535
    orig.opCount = 1;

    /* Encode orig instruct */
    error = encodeORIG(&orig, NULL, 0);

    /* Assert exit code as INVALID_CONSTANT
     * Exit code not 0 since address is invalid (>65535)
     */
    ck_assert(error == INVALID_CONSTANT);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for ORIG instruction
 *
 * @param testEncoding_ORIG_INCORRECT_NOT_WORD_ALIGNED_
 */
START_TEST (testEncoding_ORIG_INCORRECT_NOT_WORD_ALIGNED_)
{
    instruction orig;
    enum errorCode error;

    /* Initialize struct instruct */
    orig.label = NULL;
    orig.directive = "ORIG";
    orig.opcode = NULL;
    orig.operands = NULL;
    orig.addr = 12289;//0x3001, Address not word aligned
    orig.opCount = 1;

    /* Encode orig instruct */
    error = encodeORIG(&orig, NULL, 0);

    /* Assert exit code as INVALID_CONSTANT
     * Exit code not 0 since address is invalid (Not word aligned)
     */
    ck_assert(error == INVALID_CONSTANT);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for FILL instruction
 *
 * @param testEncoding_FILL_CORRECT__
 */
START_TEST (testEncoding_FILL_CORRECT_)
{
    instruction fill;
    enum errorCode error;
    char* operands[]={"#48"}; //0x30

    /* Intialize struct instruct */
    fill.label = NULL;
    fill.directive = "FILL";
    fill.opcode = NULL;
    fill.operands = operands;
    fill.addr = 12288;//0x3000
    fill.opCount = 1;

    /* Encode fill instruct */
    error = encodeFILL(&fill, NULL, 0);

    /* Assert exit code as OK_VALID */
    ck_assert(error == OK_VALID);
    ck_assert_str_eq(fill.encoding, "0000000000110000");
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for FILL instruction
 *
 * @param testEncoding_FILL_INCORRECT_LESS_THAN_0_
 */
START_TEST (testEncoding_FILL_INCORRECT_LESS_THAN_0_)
{
    instruction fill;
    enum errorCode error;
    char* operands[]={"#48"}; //0x30

    /* Initialize struct instruct */
    fill.label = NULL;
    fill.directive = "FILL";
    fill.opcode = NULL;
    fill.operands = operands;
    fill.addr = -1;//Negative address
    fill.opCount = 1;

    /* Encode fill instruct */
    error = encodeFILL(&fill, NULL, 0);

    /* Assert exit code as OTHER_ERROR
     * Exit code not 0 since address is invalid (<0)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST


/** +
 * @fn  START_TEST()
 * @brief Test case for FILL instruction
 *
 * @param testEncoding_FILL_INCORRECT_GREATER_THAN_65535_
 */
START_TEST (testEncoding_FILL_INCORRECT_GREATER_THAN_65535_)
{
    instruction fill;
    enum errorCode error;
    char* operands[]={"#48"}; //0x30

    /* Initialize struct instruct */
    fill.label = NULL;
    fill.directive = "FILL";
    fill.opcode = NULL;
    fill.operands = operands;
    fill.addr = 65536;//Address greater than 65535
    fill.opCount = 1;

    /* Encode fill instruct */
    error = encodeFILL(&fill, NULL, 0);

    /* Assert exit code as OTHER_ERROR
     * Exit code not 0 since address is invalid (>65535)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for FILL instruction
 *
 * @param testEncoding_FILL_INCORRECT_OPERAND_LESS_THAN_NEG32768_
 */
START_TEST (testEncoding_FILL_INCORRECT_OPERAND_LESS_THAN_NEG32768_)
{
    instruction fill;
    enum errorCode error;
    char* operands[]={"#-32769"};

    /* Initialize struct instruct */
    fill.label = NULL;
    fill.directive = "FILL";
    fill.opcode = NULL;
    fill.operands = operands; //Operand less than -32768
    fill.addr = 12288; //0x3000
    fill.opCount = 1;

    /* Encode fill instruct */
    error = encodeFILL(&fill, NULL, 0);
   
    /* Assert exit code as INVALID_CONSTANT
     * Exit code not 0 since operand is invalid (<-32768)
     */
    ck_assert(error == INVALID_CONSTANT);
}
END_TEST


/** +
 * @fn  START_TEST()
 * @brief Test case for FILL instruction
 *
 * @param testEncoding_FILL_INCORRECT_OPERAND_GREATER_THAN_65535_
 */
START_TEST (testEncoding_FILL_INCORRECT_OPERAND_GREATER_THAN_65535_)
{
    instruction fill;
    enum errorCode error;
    char* operands[]={"#65536"};

    /* Initialize struct instruct */
    fill.label = NULL;
    fill.directive = "FILL";
    fill.opcode = NULL;
    fill.operands = operands; //operand greater than 65535
    fill.addr = 12288;//0x3000
    fill.opCount = 1;

    /* Encode fill instruct */
    error = encodeFILL(&fill, NULL, 0);

    /* Assert exit code as INVALID_CONSTANT
     * Exit code not 0 since operand is invalid (>65535)
     */
    ck_assert(error == INVALID_CONSTANT);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for END instruction
 *
 * @param testEncoding_END_CORRECT__
 */
START_TEST (testEncoding_END_CORRECT_)
{
    instruction end;
    enum errorCode error;

    /* Intialize struct instruct */
    end.label = NULL;
    end.directive = "END";
    end.opcode = NULL;
    end.operands = NULL;
    end.addr = 12288;//0x3000
    end.opCount = 1;

    /* Encode end instruct */
    error = encodeEND(&end, NULL, 0);

    /* Assert exit code as OK_VALID */
    ck_assert(error == OK_VALID);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for NOP instruction
 *
 * @param testEncoding_NOP_CORRECT__
 */
START_TEST (testEncoding_NOP_CORRECT_)
{
    instruction nop;
    enum errorCode error;

    /* Intialize struct instruct */
    nop.label = NULL;
    nop.directive = NULL;
    nop.opcode = "NOP";
    nop.operands = NULL;
    nop.addr = 12288;//0x3000
    nop.opCount = 1;

    /* Encode nop instruct */
    error = encodeNOP(&nop, NULL, 0);

    /* Assert exit code as OK_VALID */
    ck_assert(error == OK_VALID);
    ck_assert_str_eq(nop.encoding, "0000000000000000");
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for NOP instruction
 *
 * @param testEncoding_NOP_INCORRECT_LESS_THAN_0_
 */
START_TEST (testEncoding_NOP_INCORRECT_LESS_THAN_0_)
{
    instruction nop;
    enum errorCode error;

    /* Initialize struct instruct */
    nop.label = NULL;
    nop.directive = NULL;
    nop.opcode = "NOP";
    nop.operands = NULL;
    nop.addr = -1;//Negative address
    nop.opCount = 1;

    /* Encode nop instruct */
    error = encodeNOP(&nop, NULL, 0);

    /* Assert exit code as OTHER_ERROR
     * Exit code not 0 since address is invalid (<0)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for NOP instruction
 *
 * @param testEncoding_NOP_INCORRECT_GREATER_THAN_65535_
 */
START_TEST (testEncoding_NOP_INCORRECT_GREATER_THAN_65535_)
{
    instruction nop;
    enum errorCode error;

    /* Initialize struct instruct */
    nop.label = NULL;
    nop.directive = NULL;
    nop.opcode = "NOP";
    nop.operands = NULL;
    nop.addr = 65536;//Address greater than 65535
    nop.opCount = 1;

    /* Encode nop instruct */
    error = encodeNOP(&nop, NULL, 0);

    /* Assert exit code as OTHER_ERROR
     * Exit code not 0 since address is invalid (>65535)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for TRAP instruction
 *
 * @param testEncoding_TRAP_CORRECT__
 */
START_TEST (testEncoding_TRAP_CORRECT_)
{
    instruction trap;
    enum errorCode error;
    char* operands[]={"#15"}; //0x0F

    /* Intialize struct instruct */
    trap.label = NULL;
    trap.directive = "TRAP";
    trap.opcode = NULL;
    trap.operands = operands;
    trap.addr = 12288;//0x3000
    trap.opCount = 1;

    /* Encode trap instruct */
    error = encodeTRAP(&trap, NULL, 0);

    /* Assert exit code as OK_VALID */
    ck_assert(error == OK_VALID);
    ck_assert_str_eq(trap.encoding, "1111000000001111");
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for TRAP instruction
 *
 * @param testEncoding_TRAP_INCORRECT_OPERAND_LESS_THAN_0_
 */
START_TEST (testEncoding_TRAP_INCORRECT_OPERAND_LESS_THAN_0_)
{
    instruction trap;
    enum errorCode error;
    char* operands[]={"#-1"};

    /* Initialize struct instruct */
    trap.label = NULL;
    trap.directive = "TRAP";
    trap.opcode = NULL;
    trap.operands = operands; //Operand less than -32768
    trap.addr = 12288; //0x3000
    trap.opCount = 1;

    /* Encode trap instruct */
    error = encodeTRAP(&trap, NULL, 0);
   
    /* Assert exit code as INVALID_CONSTANT
     * Exit code not 0 since operand is invalid (<0)
     */
    ck_assert(error == INVALID_CONSTANT);
}
END_TEST


/** +
 * @fn  START_TEST()
 * @brief Test case for TRAP instruction
 *
 * @param testEncoding_TRAP_INCORRECT_OPERAND_GREATER_THAN_255_
 */
START_TEST (testEncoding_TRAP_INCORRECT_OPERAND_GREATER_THAN_255_)
{
    instruction trap;
    enum errorCode error;
    char* operands[]={"#256"};

    /* Initialize struct instruct */
    trap.label = NULL;
    trap.directive = "TRAP";
    trap.opcode = NULL;
    trap.operands = operands; //operand greater than 65535
    trap.addr = 12288;//0x3000
    trap.opCount = 1;

    /* Encode fill instruct */
    error = encodeTRAP(&trap, NULL, 0);

    /* Assert exit code as INVALID_CONSTANT
     * Exit code not 0 since operand is invalid (>255)
     */
    ck_assert(error == INVALID_CONSTANT);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for HALT instruction
 *
 * @param testEncoding_HALT_CORRECT__
 */
START_TEST (testEncoding_HALT_CORRECT_)
{
    instruction halt;
    enum errorCode error;

    /* Intialize struct instruct */
    halt.label = NULL;
    halt.directive = "HALT";
    halt.opcode = NULL;
    halt.operands = NULL;
    halt.addr = 12288;//0x3000
    halt.opCount = 1;

    /* Encode halt instruct */
    error = encodeHALT(&halt, NULL, 0);

    /* Assert exit code as OK_VALID */
    ck_assert(error == OK_VALID);
    ck_assert_str_eq(halt.encoding, "1111000000100101");
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for HALT instruction
 *
 * @param testEncoding_HALT_INCORRECT_LESS_THAN_0_
 */
START_TEST (testEncoding_HALT_INCORRECT_LESS_THAN_0_)
{
    instruction halt;
    enum errorCode error;

    /* Initialize struct instruct */
    halt.label = NULL;
    halt.directive = "HALT";
    halt.opcode = NULL;
    halt.operands = NULL;
    halt.addr = -1;//Negative address
    halt.opCount = 1;

    /* Encode halt instruct */
    error = encodeHALT(&halt, NULL, 0);

    /* Assert exit code as INVALID_CONSTANT
     * Exit code not 0 since address is invalid (<0)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for HALT instruction
 *
 * @param testEncoding_HALT_INCORRECT_GREATER_THAN_65535_
 */
START_TEST (testEncoding_HALT_INCORRECT_GREATER_THAN_65535_)
{
    instruction halt;
    enum errorCode error;

    /* Initialize struct instruct */
    halt.label = NULL;
    halt.directive = "HALT";
    halt.opcode = NULL;
    halt.operands = NULL;
    halt.addr = 65536;//Address greater than 65535
    halt.opCount = 1;

    /* Encode halt instruct */
    error = encodeHALT(&halt, NULL, 0);

    /* Assert exit code as INVALID_CONSTANT
     * Exit code not 0 since address is invalid (>65535)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for ADD instruction
 *
 * @param testEncoding_ADD_CORRECT__
 */
START_TEST (testEncoding_ADD_CORRECT_)
{
    instruction add;
    enum errorCode error;
    char* operands[]={"R3", "R3", "#1"};

    /* Intialize struct instruct */
    add.label = NULL;
    add.directive = NULL;
    add.opcode = "ADD";
    add.operands = operands;
    add.addr = 12288;//0x3000
    add.opCount = 1;

    /* Encode add instruct */
    error = encodeADD(&add, NULL, 0);

    /* Assert exit code as OK_VALID */
    ck_assert(error == OK_VALID);
    ck_assert_str_eq(add.encoding, "0001011011100001");
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for ADD instruction
 *
 * @param testEncoding_ADD_INCORRECT_LESS_THAN_0_
 */
START_TEST (testEncoding_ADD_INCORRECT_LESS_THAN_0_)
{
    instruction add;
    enum errorCode error;
    char* operands[]={"R3", "R3", "#1"};

    /* Initialize struct instruct */
    add.label = NULL;
    add.directive = NULL;
    add.opcode = "ADD";
    add.operands = operands;
    add.addr = -1;//Negative address
    add.opCount = 1;

    /* Encode add instruct */
    error = encodeADD(&add, NULL, 0);

    /* Assert exit code as OTHER_ERROR
     * Exit code not 0 since address is invalid (<0)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for ADD instruction
 *
 * @param testEncoding_ADD_INCORRECT_GREATER_THAN_65535_
 */
START_TEST (testEncoding_ADD_INCORRECT_GREATER_THAN_65535_)
{
    instruction add;
    enum errorCode error;
    char* operands[]={"R3", "R3", "#1"};

    /* Initialize struct instruct */
    add.label = NULL;
    add.directive = NULL;
    add.opcode = "ADD";
    add.operands = operands;
    add.addr = 65536;//Address greater than 65535
    add.opCount = 1;

    /* Encode add instruct */
    error = encodeADD(&add, NULL, 0);

    /* Assert exit code as OTHER_ERROR
     * Exit code not 0 since address is invalid (>65535)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST


/** +
 * @fn int main(void)
 * @brief main method to initialize and create test suite
 *
 * @return int exit code
 */
int main(void)
{
    /* Create test suite for testing the assembler */
    Suite *s1 = suite_create("Test Encoding");

    /* Create test cases to add to test suite */
    TCase *tc1_0 = tcase_create("ORIG CORRECT");
    TCase *tc1_1 = tcase_create("ORIG INCORRECT ADDRESS LESS THAN 0");
    TCase *tc1_2 = tcase_create("ORIG INCORRECT ADDRESS GREATER THAN 65535");
    TCase *tc1_3 = tcase_create("ORIG INCORRECT ADDRESS NOT WORD ALIGNED");

    TCase *tc1_4 = tcase_create("FILL CORRECT");
    TCase *tc1_5 = tcase_create("FILL INCORRECT ADDRESS LESS THAN 0");
    TCase *tc1_6 = tcase_create("FILL INCORRECT ADDRESS GREATER THAN 65535");
    TCase *tc1_7 = tcase_create("FILL INCORRECT OPERAND LESS THAN -32768");
    TCase *tc1_8 = tcase_create("FILL INCORRECT OPERAND GREATER THAN 65535");

    TCase *tc1_9 = tcase_create("END CORRECT");

    TCase *tc1_10 = tcase_create("NOP CORRECT");
    TCase *tc1_11 = tcase_create("NOP INCORRECT ADDRESS LESS THAN 0");
    TCase *tc1_12 = tcase_create("NOP INCORRECT ADDRESS GREATER THAN 65535");
    
    TCase *tc1_13 = tcase_create("TRAP CORRECT");
    TCase *tc1_14 = tcase_create("TRAP INCORRECT OPERAND LESS THAN 0");
    TCase *tc1_15 = tcase_create("TRAP INCORRECT OPERAND GREATER THAN 255");

    TCase *tc1_16 = tcase_create("HALT CORRECT");
    TCase *tc1_17 = tcase_create("HALT INCORRECT ADDRESS LESS THAN 0");
    TCase *tc1_18 = tcase_create("HALT INCORRECT ADDRESS GREATER THAN 65535");

    TCase *tc1_19 = tcase_create("ADD CORRECT");
    TCase *tc1_20 = tcase_create("ADD INCORRECT ADDRESS LESS THAN 0");
    TCase *tc1_21 = tcase_create("ADD INCORRECT ADDRESS GREATER THAN 65535");
    
    /* Create test runner to run the test */
    SRunner *sr = srunner_create(s1);

    /* Number of pass-fails*/
    int nf;

    /* Add test cases to test suite */
    suite_add_tcase(s1, tc1_0);
    suite_add_tcase(s1, tc1_1);
    suite_add_tcase(s1, tc1_2);
    suite_add_tcase(s1, tc1_3);

    suite_add_tcase(s1, tc1_4);
    suite_add_tcase(s1, tc1_5);
    suite_add_tcase(s1, tc1_6);
    suite_add_tcase(s1, tc1_7);
    suite_add_tcase(s1, tc1_8);

    suite_add_tcase(s1, tc1_9);

    suite_add_tcase(s1, tc1_10);
    suite_add_tcase(s1, tc1_11);
    suite_add_tcase(s1, tc1_12);
    
    suite_add_tcase(s1, tc1_13);
    suite_add_tcase(s1, tc1_14);
    suite_add_tcase(s1, tc1_15);

    suite_add_tcase(s1, tc1_16);
    suite_add_tcase(s1, tc1_17);
    suite_add_tcase(s1, tc1_18);

    suite_add_tcase(s1, tc1_19);
    suite_add_tcase(s1, tc1_20);
    suite_add_tcase(s1, tc1_21);
    
    /* Add test methods to test cases */
    tcase_add_test(tc1_0, testEncoding_ORIG_CORRECT_);
    tcase_add_test(tc1_1, testEncoding_ORIG_INCORRECT_LESS_THAN_0_);
    tcase_add_test(tc1_2, testEncoding_ORIG_INCORRECT_GREATER_THAN_65535_);
    tcase_add_test(tc1_3, testEncoding_ORIG_INCORRECT_NOT_WORD_ALIGNED_);

    tcase_add_test(tc1_4, testEncoding_FILL_CORRECT_);
    tcase_add_test(tc1_5, testEncoding_FILL_INCORRECT_LESS_THAN_0_);
    tcase_add_test(tc1_6, testEncoding_FILL_INCORRECT_GREATER_THAN_65535_);
    tcase_add_test(tc1_7, testEncoding_FILL_INCORRECT_OPERAND_LESS_THAN_NEG32768_);
    tcase_add_test(tc1_8, testEncoding_FILL_INCORRECT_OPERAND_GREATER_THAN_65535_);

    tcase_add_test(tc1_9, testEncoding_END_CORRECT_);

    tcase_add_test(tc1_10, testEncoding_NOP_CORRECT_);
    tcase_add_test(tc1_11, testEncoding_NOP_INCORRECT_LESS_THAN_0_);
    tcase_add_test(tc1_12, testEncoding_NOP_INCORRECT_GREATER_THAN_65535_);

    tcase_add_test(tc1_13, testEncoding_TRAP_CORRECT_);
    tcase_add_test(tc1_14, testEncoding_TRAP_INCORRECT_OPERAND_LESS_THAN_0_);
    tcase_add_test(tc1_15, testEncoding_TRAP_INCORRECT_OPERAND_GREATER_THAN_255_);

    tcase_add_test(tc1_16, testEncoding_HALT_CORRECT_);
    tcase_add_test(tc1_17, testEncoding_HALT_INCORRECT_LESS_THAN_0_);
    tcase_add_test(tc1_18, testEncoding_HALT_INCORRECT_GREATER_THAN_65535_);

    tcase_add_test(tc1_19, testEncoding_ADD_CORRECT_);
    tcase_add_test(tc1_20, testEncoding_ADD_INCORRECT_LESS_THAN_0_);
    tcase_add_test(tc1_21, testEncoding_ADD_INCORRECT_GREATER_THAN_65535_);

    /* Run all test cases in test suite*/
    srunner_run_all(sr, CK_ENV);

    /* Get number of pass-fails */
    nf = srunner_ntests_failed(sr);

    /* Free memory allocated to test suite */
    srunner_free(sr);

    /* Exit with code 0 if all tests passed*/
    return nf == 0 ? 0 : 1;
}



