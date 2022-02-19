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
 * @fn int main(void)
 * @brief main method to initialize and create test suite
 *
 * @return int exit code
 */
int main(void)
{
    /* Create test suite for testing the assembler */
    Suite *s1 = suite_create("Test ORIG");

    /* Create test cases to add to test suite */
    TCase *tc1_0 = tcase_create("ORIG CORRECT");
    TCase *tc1_1 = tcase_create("ORIG INCORRECT ADDRESS LESS THAN 0");
    TCase *tc1_2 = tcase_create("ORIG INCORRECT ADDRESS GREATER THAN 65535");
    TCase *tc1_3 = tcase_create("ORIG INCORRECT ADDRESS NOT WORD ALIGNED");

    /* Create test runner to run the test */
    SRunner *sr = srunner_create(s1);

    /* Number of pass-fails*/
    int nf;

    /* Add test cases to test suite */
    suite_add_tcase(s1, tc1_0);
    suite_add_tcase(s1, tc1_1);
    suite_add_tcase(s1, tc1_2);
    suite_add_tcase(s1, tc1_3);

    /* Add test methods to test cases */
    tcase_add_test(tc1_0, testEncoding_ORIG_CORRECT_);
    tcase_add_test(tc1_1, testEncoding_ORIG_INCORRECT_LESS_THAN_0_);
    tcase_add_test(tc1_2, testEncoding_ORIG_INCORRECT_GREATER_THAN_65535_);
    tcase_add_test(tc1_3, testEncoding_ORIG_INCORRECT_NOT_WORD_ALIGNED_);

    /* Run all test cases in test suite*/
    srunner_run_all(sr, CK_ENV);

    /* Get number of pass-fails */
    nf = srunner_ntests_failed(sr);

    /* Free memory allocated to test suite */
    srunner_free(sr);

    /* Exit with code 0 if all tests passed*/
    return nf == 0 ? 0 : 1;
}
