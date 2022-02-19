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
 * @fn int main(void)
 * @brief main method to initialize and create test suite
 *
 * @return int exit code
 */
int main(void)
{
    /* Create test suite for testing the assembler */
    Suite *s1 = suite_create("Test END");

    /* Create test cases to add to test suite */
    TCase *tc1_0 = tcase_create("END CORRECT");
    
    /* Create test runner to run the test */
    SRunner *sr = srunner_create(s1);

    /* Number of pass-fails*/
    int nf;

    /* Add test cases to test suite */
    suite_add_tcase(s1, tc1_0);
    
    /* Add test methods to test cases */
    tcase_add_test(tc1_0, testEncoding_END_CORRECT_);

    /* Run all test cases in test suite*/
    srunner_run_all(sr, CK_ENV);

    /* Get number of pass-fails */
    nf = srunner_ntests_failed(sr);

    /* Free memory allocated to test suite */
    srunner_free(sr);

    /* Exit with code 0 if all tests passed*/
    return nf == 0 ? 0 : 1;
}
