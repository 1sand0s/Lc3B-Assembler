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
 * @brief Test case for BRNZP instruction
 *
 * @param testEncoding_BRNZP_CORRECT__
 */
START_TEST (testEncoding_BRNZP_CORRECT_)
{
    instruction brnzp;
    symbol label;
    enum errorCode error;
    char* operands[]={"A"};

    /* Initialize struct symbol */
    label.symbolName = "A";
    label.addr = 12300;
    
    /* Intialize struct instruct */
    brnzp.label = NULL;
    brnzp.directive = NULL;
    brnzp.opcode = "BRNZP";
    brnzp.operands = operands;
    brnzp.addr = 12288;//0x3000
    brnzp.opCount = 1;

    /* Encode brnzp instruct */
    error = encodeBRNZP(&brnzp, &label, 1);

    /* Assert exit code as OK_VALID */
    ck_assert(error == OK_VALID);
    ck_assert_str_eq(brnzp.encoding, "0000111000000101");
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for BRNZP instruction
 *
 * @param testEncoding_BRNZP_INCORRECT_LESS_THAN_0_
 */
START_TEST (testEncoding_BRNZP_INCORRECT_LESS_THAN_0_)
{
    instruction brnzp;
    symbol label;
    enum errorCode error;
    char* operands[]={"A"};

    /* Initialize struct symbol */
    label.symbolName = "A";
    label.addr = 12300;
    
    /* Initialize struct instruct */
    brnzp.label = NULL;
    brnzp.directive = NULL;
    brnzp.opcode = "BRNZP";
    brnzp.operands = operands;
    brnzp.addr = -1;//Negative address
    brnzp.opCount = 1;

    /* Encode brnzp instruct */
    error = encodeBRNZP(&brnzp, &label, 1);

    /* Assert exit code as OTHER_ERROR
     * Exit code not 0 since address is invalid (<0)
     */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for BRNZP instruction
 *
 * @param testEncoding_BRNZP_INCORRECT_GREATER_THAN_65535_
 */
START_TEST (testEncoding_BRNZP_INCORRECT_GREATER_THAN_65535_)
{
    instruction brnzp;
    symbol label;
    enum errorCode error;
    char* operands[]={"A"};
	
    /* Initialize struct symbol */
    label.symbolName = "A";
    label.addr = 12300;
    
    /* Initialize struct instruct */
    brnzp.label = NULL;
    brnzp.directive = NULL;
    brnzp.opcode = "BRNZP";
    brnzp.operands = operands;
    brnzp.addr = 65536;//Address greater than 65535
    brnzp.opCount = 1;

    /* Encode brnzp instruct */
    error = encodeBRNZP(&brnzp, &label, 1);

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
    Suite *s1 = suite_create("Test BRNZP");

    /* Create test cases to add to test suite */
    TCase *tc1_0 = tcase_create("BRNZP CORRECT");
    TCase *tc1_1 = tcase_create("BRNZP INCORRECT ADDRESS LESS THAN 0");
    TCase *tc1_2 = tcase_create("BRNZP INCORRECT ADDRESS GREATER THAN 65535");
    
    /* Create test runner to run the test */
    SRunner *sr = srunner_create(s1);

    /* Number of pass-fails*/
    int nf;

    /* Add test cases to test suite */
    suite_add_tcase(s1, tc1_0);
    suite_add_tcase(s1, tc1_1);
    suite_add_tcase(s1, tc1_2);
    
    /* Add test methods to test cases */
    tcase_add_test(tc1_0, testEncoding_BRNZP_CORRECT_);
    tcase_add_test(tc1_1, testEncoding_BRNZP_INCORRECT_LESS_THAN_0_);
    tcase_add_test(tc1_2, testEncoding_BRNZP_INCORRECT_GREATER_THAN_65535_);

    /* Run all test cases in test suite*/
    srunner_run_all(sr, CK_ENV);

    /* Get number of pass-fails */
    nf = srunner_ntests_failed(sr);

    /* Free memory allocated to test suite */
    srunner_free(sr);

    /* Exit with code 0 if all tests passed*/
    return nf == 0 ? 0 : 1;
}
