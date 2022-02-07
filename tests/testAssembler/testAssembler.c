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

START_TEST (testAssembler)
{
    FILE *in, *out;
    enum errorCode error;
    char *line = NULL;
    size_t len = 0;
    char **actual;
    int actual_count = 0;

    /* Expected translation */
    char *expected[] = {"0x1000\n",
                        "0x1260\n"};

    /* Open/create files for reading ASM code and writing HEX code */
    in = fopen("testCases/prog1.asm", "r");

    /* Opening <*.hex> in writing and reading mode for rewinding later */
    out = fopen("prog1.hex", "w+");

    /* Assemble the ASM file and write HEX to out file */
    error = assemble(in, out);
    fclose(in);
    
    /* Rewind <*.hex> file back to start to extract lines */
    rewind(out);
    
    /* Extract contents of HEX file line by line 
     * And store in char **actual
     */
    while(getline( & line, & len, out) != -1){
        if(actual_count == 0)
            actual = malloc(sizeof(char*));
        else
            actual = realloc(actual, (actual_count + 1)*sizeof(char*));

        actual[actual_count]=malloc((len+1)*sizeof(char));
        strncpy(actual[actual_count],line,(len+1));
        actual_count++;
    }
    fclose(out);

    /* Free buffer used to store lines of the generated  <*.hex> file */
    free(line);

    /* Assert if contents of expected and actual match */
    ck_assert(error == OK_VALID);
    ck_assert_int_eq(actual_count, 2);
    ck_assert_str_eq(expected[0], actual[0]);
    ck_assert_str_eq(expected[1], actual[1]);
}
END_TEST

int main(void)
{
    Suite *s1 = suite_create("Core");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_test(tc1_1, testAssembler);

    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? 0 : 1;
}



