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
 * @brief Test case for BR instruction
 *
 * @param testEncoding_BR_CORRECT__
 */
START_TEST (testEncoding_BR_CORRECT_)
{
    instruction br;
    symbol label;
    enum errorCode error;
    char* operands[]={"A"};

    /* Initialize struct symbol */
    label.symbolName = "A";
    label.addr = 12300;
    
    /* Intialize struct instruct */
    br.label = NULL;
    br.directive = NULL;
    br.opcode = "BR";
    br.operands = operands;
    br.addr = 12288;//0x3000
    br.opCount = 1;

    /* Encode br instruct */
    error = encodeBR(&br, &label, 1);

    /* Assert exit code as OK_VALID */
    ck_assert(error == OK_VALID);
    ck_assert_str_eq(br.encoding, "0000111000000101");
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for BR instruction
 *
 * @param testEncoding_BR_INCORRECT_LESS_THAN_0_
 */
START_TEST (testEncoding_BR_INCORRECT_LESS_THAN_0_)
{
    instruction br;
    symbol label;
    enum errorCode error;
    char* operands[]={"A"};

    /* Initialize struct symbol */
    label.symbolName = "A";
    label.addr = 12300;
    
    /* Initialize struct instruct */
    br.label = NULL;
    br.directive = NULL;
    br.opcode = "BR";
    br.operands = operands;
    br.addr = -1;//Negative address
    br.opCount = 1;

    /* Encode br instruct */
    error = encodeBR(&br, &label, 1);

    /* Assert exit code as OTHER_ERROR
     * Exit code not 0 since address is invalid (<0)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for BR instruction
 *
 * @param testEncoding_BR_INCORRECT_GREATER_THAN_65535_
 */
START_TEST (testEncoding_BR_INCORRECT_GREATER_THAN_65535_)
{
    instruction br;
    symbol label;
    enum errorCode error;
    char* operands[]={"A"};
	
    /* Initialize struct symbol */
    label.symbolName = "A";
    label.addr = 12300;
    
    /* Initialize struct instruct */
    br.label = NULL;
    br.directive = NULL;
    br.opcode = "BR";
    br.operands = operands;
    br.addr = 65536;//Address greater than 65535
    br.opCount = 1;

    /* Encode br instruct */
    error = encodeBR(&br, &label, 1);

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
    Suite *s1 = suite_create("Test BR");

    /* Create test cases to add to test suite */
    TCase *tc1_0 = tcase_create("BR CORRECT");
    TCase *tc1_1 = tcase_create("BR INCORRECT ADDRESS LESS THAN 0");
    TCase *tc1_2 = tcase_create("BR INCORRECT ADDRESS GREATER THAN 65535");
    
    /* Create test runner to run the test */
    SRunner *sr = srunner_create(s1);

    /* Number of pass-fails*/
    int nf;

    /* Add test cases to test suite */
    suite_add_tcase(s1, tc1_0);
    suite_add_tcase(s1, tc1_1);
    suite_add_tcase(s1, tc1_2);
    
    /* Add test methods to test cases */
    tcase_add_test(tc1_0, testEncoding_BR_CORRECT_);
    tcase_add_test(tc1_1, testEncoding_BR_INCORRECT_LESS_THAN_0_);
    tcase_add_test(tc1_2, testEncoding_BR_INCORRECT_GREATER_THAN_65535_);

    /* Run all test cases in test suite*/
    srunner_run_all(sr, CK_ENV);

    /* Get number of pass-fails */
    nf = srunner_ntests_failed(sr);

    /* Free memory allocated to test suite */
    srunner_free(sr);

    /* Exit with code 0 if all tests passed*/
    return nf == 0 ? 0 : 1;
}
