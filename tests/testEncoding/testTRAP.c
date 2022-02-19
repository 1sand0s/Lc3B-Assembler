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
 * @fn int main(void)
 * @brief main method to initialize and create test suite
 *
 * @return int exit code
 */
int main(void)
{
    /* Create test suite for testing the assembler */
    Suite *s1 = suite_create("Test TRAP");

    /* Create test cases to add to test suite */
    TCase *tc1_0 = tcase_create("TRAP CORRECT");
    TCase *tc1_1 = tcase_create("TRAP INCORRECT OPERAND LESS THAN 0");
    TCase *tc1_2 = tcase_create("TRAP INCORRECT OPERAND GREATER THAN 255");
    
    /* Create test runner to run the test */
    SRunner *sr = srunner_create(s1);

    /* Number of pass-fails*/
    int nf;

    /* Add test cases to test suite */
    suite_add_tcase(s1, tc1_0);
    suite_add_tcase(s1, tc1_1);
    suite_add_tcase(s1, tc1_2);
    
    /* Add test methods to test cases */
    tcase_add_test(tc1_0, testEncoding_TRAP_CORRECT_);
    tcase_add_test(tc1_1, testEncoding_TRAP_INCORRECT_OPERAND_LESS_THAN_0_);
    tcase_add_test(tc1_2, testEncoding_TRAP_INCORRECT_OPERAND_GREATER_THAN_255_);

    /* Run all test cases in test suite*/
    srunner_run_all(sr, CK_ENV);

    /* Get number of pass-fails */
    nf = srunner_ntests_failed(sr);

    /* Free memory allocated to test suite */
    srunner_free(sr);

    /* Exit with code 0 if all tests passed*/
    return nf == 0 ? 0 : 1;
}
