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
 * @fn int main(void)
 * @brief main method to initialize and create test suite
 *
 * @return int exit code
 */
int main(void)
{
    /* Create test suite for testing the assembler */
    Suite *s1 = suite_create("Test FILL");

    /* Create test cases to add to test suite */
    TCase *tc1_0 = tcase_create("FILL CORRECT");
    TCase *tc1_1 = tcase_create("FILL INCORRECT ADDRESS LESS THAN 0");
    TCase *tc1_2 = tcase_create("FILL INCORRECT ADDRESS GREATER THAN 65535");
    TCase *tc1_3 = tcase_create("FILL INCORRECT OPERAND LESS THAN -32768");
    TCase *tc1_4 = tcase_create("FILL INCORRECT OPERAND GREATER THAN 65535");

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
    
    /* Add test methods to test cases */
    tcase_add_test(tc1_0, testEncoding_FILL_CORRECT_);
    tcase_add_test(tc1_1, testEncoding_FILL_INCORRECT_LESS_THAN_0_);
    tcase_add_test(tc1_2, testEncoding_FILL_INCORRECT_GREATER_THAN_65535_);
    tcase_add_test(tc1_3, testEncoding_FILL_INCORRECT_OPERAND_LESS_THAN_NEG32768_);
    tcase_add_test(tc1_4, testEncoding_FILL_INCORRECT_OPERAND_GREATER_THAN_65535_);

    /* Run all test cases in test suite*/
    srunner_run_all(sr, CK_ENV);

    /* Get number of pass-fails */
    nf = srunner_ntests_failed(sr);

    /* Free memory allocated to test suite */
    srunner_free(sr);

    /* Exit with code 0 if all tests passed*/
    return nf == 0 ? 0 : 1;
}
