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
 * @def PROG0_ASM_LINES_
 * @brief Number of lines in prog.hex
 *
 */
#define PROG0_ASM_LINES_ 13

/** +
 * @def PROG1_ASM_LINES_
 * @brief Number of lines in prog1.hex
 *
 */
#define PROG1_ASM_LINES_ 2

/** +
 * @def PROG2_ASM_LINES_
 * @brief Number of lines in prog2.hex
 *
 */
#define PROG2_ASM_LINES_ 9

/** +
 * @def PROG3_ASM_LINES_
 * @brief Number of lines in prog3.hex
 *
 */
#define PROG3_ASM_LINES_ 8

/** +
 * @def PROG6_ASM_LINES_
 * @brief Number of lines in prog6.hex
 *
 */
#define PROG6_ASM_LINES_ 1

/** +
 * @def PROG9_ASM_LINES_
 * @brief Number of lines in prog9.hex
 *
 */
#define PROG9_ASM_LINES_ 21

/** +
 * @def SHUFFLE_ASM_LINES_
 * @brief Number of lines in shuffle.hex
 *
 */
#define SHUFFLE_ASM_LINES_ 12

/** +
 * @fn  START_TEST()
 * @brief Test case for prog.asm
 *
 * @param testAssembler_prog0_
 */
START_TEST (testAssembler_prog0_)
{
    FILE *in, *out;
    enum errorCode error;
    char *line = NULL;
    size_t len = 0;
    char **actual;
    int actual_count = 0;

    /* Expected translation */
    char *expected[] = {"0x3000\n",
			"0xE005\n",
			"0x621F\n",
			"0xD25F\n",
			"0x0401\n",
			"0x0FFD\n",
			"0xF025\n",
			"0xFFF6\n",
			"0x0000\n",
			"0xC1C0\n",
			"0xC000\n",
			"0x4FFA\n",
			"0x4180\n"};

    /* Open/create files for reading ASM code and writing HEX code */
    in = fopen("testCases/prog.asm", "r");

    /* Opening <*.hex> in writing and reading mode for rewinding later */
    out = fopen("prog.hex", "w+");

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
    ck_assert_int_eq(actual_count, PROG0_ASM_LINES_);
    ck_assert_str_eq(expected[0], actual[0]);
    ck_assert_str_eq(expected[1], actual[1]);
    ck_assert_str_eq(expected[2], actual[2]);
    ck_assert_str_eq(expected[3], actual[3]);
    ck_assert_str_eq(expected[4], actual[4]);
    ck_assert_str_eq(expected[5], actual[5]);
    ck_assert_str_eq(expected[6], actual[6]);
    ck_assert_str_eq(expected[7], actual[7]);
    ck_assert_str_eq(expected[8], actual[8]);
    ck_assert_str_eq(expected[9], actual[9]);
    ck_assert_str_eq(expected[10], actual[10]);
    ck_assert_str_eq(expected[11], actual[11]);
    ck_assert_str_eq(expected[12], actual[12]);

    /* Free memory for char* contained within actual */
    for(int j = 0; j < actual_count; j++)
      free(actual[j]);

    /* Free memory of char** */
    free(actual);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for prog1.asm
 *
 * @param testAssembler_prog1_
 */
START_TEST (testAssembler_prog1_)
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
    ck_assert_int_eq(actual_count, PROG1_ASM_LINES_);
    ck_assert_str_eq(expected[0], actual[0]);
    ck_assert_str_eq(expected[1], actual[1]);
    
    /* Free memory for char* contained within actual */
    for(int j = 0; j < actual_count; j++)
      free(actual[j]);

    /* Free memory of char** */
    free(actual);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for prog2.asm
 *
 * @param testAssembler_prog2_
 */
START_TEST (testAssembler_prog2_)
{
    FILE *in, *out;
    enum errorCode error;
    char *line = NULL;
    size_t len = 0;
    char **actual;
    int actual_count = 0;

    /* Expected translation */
    char *expected[] = {"0x4000\n",
			"0xE403\n",
			"0x4080\n",
			"0x4803\n",
			"0xF025\n",
			"0x1025\n",
			"0xC1C0\n",
			"0x1265\n",
			"0xC1C0\n"};


    /* Open/create files for reading ASM code and writing HEX code */
    in = fopen("testCases/prog2.asm", "r");

    /* Opening <*.hex> in writing and reading mode for rewinding later */
    out = fopen("prog2.hex", "w+");

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
    ck_assert_int_eq(actual_count, PROG2_ASM_LINES_);
    ck_assert_str_eq(expected[0], actual[0]);
    ck_assert_str_eq(expected[1], actual[1]);
    ck_assert_str_eq(expected[2], actual[2]);
    ck_assert_str_eq(expected[3], actual[3]);
    ck_assert_str_eq(expected[4], actual[4]);
    ck_assert_str_eq(expected[5], actual[5]);
    ck_assert_str_eq(expected[6], actual[6]);
    ck_assert_str_eq(expected[7], actual[7]);
    ck_assert_str_eq(expected[8], actual[8]);

    /* Free memory for char* contained within actual */
    for(int j = 0; j < actual_count; j++)
      free(actual[j]);

    /* Free memory of char** */
    free(actual);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for prog3.asm
 *
 * @param testAssembler_prog3_
 */
START_TEST (testAssembler_prog3_)
{
    FILE *in, *out;
    enum errorCode error;
    char *line = NULL;
    size_t len = 0;
    char **actual;
    int actual_count = 0;

    /* Expected translation */
    char *expected[] = {"0x3000\n",
			"0xE005\n",
			"0x6200\n",
			"0x127F\n",
			"0x0401\n",
			"0x0FFD\n",
			"0xF025\n",
			"0x000A\n"};


    /* Open/create files for reading ASM code and writing HEX code */
    in = fopen("testCases/prog3.asm", "r");

    /* Opening <*.hex> in writing and reading mode for rewinding later */
    out = fopen("prog3.hex", "w+");

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
    ck_assert_int_eq(actual_count, PROG3_ASM_LINES_);
    ck_assert_str_eq(expected[0], actual[0]);
    ck_assert_str_eq(expected[1], actual[1]);
    ck_assert_str_eq(expected[2], actual[2]);
    ck_assert_str_eq(expected[3], actual[3]);
    ck_assert_str_eq(expected[4], actual[4]);
    ck_assert_str_eq(expected[5], actual[5]);
    ck_assert_str_eq(expected[6], actual[6]);
    ck_assert_str_eq(expected[7], actual[7]);
    
    /* Free memory for char* contained within actual */
    for(int j = 0; j < actual_count; j++)
      free(actual[j]);

    /* Free memory of char** */
    free(actual);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for prog4.asm
 *
 * @param testAssembler_prog4_
 */
START_TEST (testAssembler_prog4_)
{
    FILE *in, *out;
    enum errorCode error;

    /* Open/create files for reading ASM code and writing HEX code */
    in = fopen("testCases/prog4.asm", "r");

    /* Opening <*.hex> in writing and reading mode for rewinding later */
    out = fopen("prog4.hex", "w+");

    /* Assemble the ASM file and write HEX to out file */
    error = assemble(in, out);
    fclose(in);
    fclose(out);

    /* Assert if contents of expected and actual match */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for prog6.asm
 *
 * @param testAssembler_prog6_
 */
START_TEST (testAssembler_prog6_)
{
    FILE *in, *out;
    enum errorCode error;
    char *line = NULL;
    size_t len = 0;
    char **actual;
    int actual_count = 0;

    /* Expected translation */
    char *expected = "0x3000\n";


    /* Open/create files for reading ASM code and writing HEX code */
    in = fopen("testCases/prog6.asm", "r");

    /* Opening <*.hex> in writing and reading mode for rewinding later */
    out = fopen("prog6.hex", "w+");

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
    ck_assert_int_eq(actual_count, PROG6_ASM_LINES_);
    ck_assert_str_eq(expected, actual[0]);
    
    /* Free memory for char* contained within actual */
    for(int j = 0; j < actual_count; j++)
      free(actual[j]);

    /* Free memory of char** */
    free(actual);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for prog7.asm
 *
 * @param testAssembler_prog7_
 */
START_TEST (testAssembler_prog7_)
{
    FILE *in, *out;
    enum errorCode error;

    /* Open/create files for reading ASM code and writing HEX code */
    in = fopen("testCases/prog7.asm", "r");

    /* Opening <*.hex> in writing and reading mode for rewinding later */
    out = fopen("prog7.hex", "w+");

    /* Assemble the ASM file and write HEX to out file */
    error = assemble(in, out);
    fclose(in);
    fclose(out);

    /* Assert if contents of expected and actual match */
    ck_assert(error == OTHER_ERROR);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for prog9.asm
 *
 * @param testAssembler_prog9_
 */
START_TEST (testAssembler_prog9_)
{
    FILE *in, *out;
    enum errorCode error;
    char *line = NULL;
    size_t len = 0;
    char **actual;
    int actual_count = 0;

    /* Expected translation */
    char *expected[] = {"0x3000\n",
			"0x5B60\n",
			"0x56E0\n",
			"0x16E8\n",
			"0xE00E\n",
			"0x6201\n",
			"0x6240\n",
			"0x1460\n",
			"0x1482\n",
			"0x16FF\n",
			"0x03FD\n",
			"0x6800\n",
			"0x5244\n",
			"0x927F\n",
			"0x1261\n",
			"0x1481\n",
			"0x0A01\n",
			"0x1B61\n",
			"0xF025\n",
			"0xFF00\n",
			"0x4000\n"};


    /* Open/create files for reading ASM code and writing HEX code */
    in = fopen("testCases/prog9.asm", "r");

    /* Opening <*.hex> in writing and reading mode for rewinding later */
    out = fopen("prog9.hex", "w+");

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
    ck_assert_int_eq(actual_count, PROG9_ASM_LINES_);
    ck_assert_str_eq(expected[0], actual[0]);
    ck_assert_str_eq(expected[1], actual[1]);
    ck_assert_str_eq(expected[2], actual[2]);
    ck_assert_str_eq(expected[3], actual[3]);
    ck_assert_str_eq(expected[4], actual[4]);
    ck_assert_str_eq(expected[5], actual[5]);
    ck_assert_str_eq(expected[6], actual[6]);
    ck_assert_str_eq(expected[7], actual[7]);
    ck_assert_str_eq(expected[8], actual[8]);
    ck_assert_str_eq(expected[9], actual[9]);
    ck_assert_str_eq(expected[10], actual[10]);
    ck_assert_str_eq(expected[11], actual[11]);
    ck_assert_str_eq(expected[12], actual[12]);
    ck_assert_str_eq(expected[13], actual[13]);
    ck_assert_str_eq(expected[14], actual[14]);
    ck_assert_str_eq(expected[15], actual[15]);
    ck_assert_str_eq(expected[16], actual[16]);
    ck_assert_str_eq(expected[17], actual[17]);
    ck_assert_str_eq(expected[18], actual[18]);
    ck_assert_str_eq(expected[19], actual[19]);
    ck_assert_str_eq(expected[20], actual[20]);
    
    /* Free memory for char* contained within actual */
    for(int j = 0; j < actual_count; j++)
      free(actual[j]);

    /* Free memory of char** */
    free(actual);
}
END_TEST

/** +
 * @fn  START_TEST()
 * @brief Test case for shuffle.asm
 *
 * @param testAssembler_shuffle_
 */
START_TEST (testAssembler_shuffle_)
{
    FILE *in, *out;
    enum errorCode error;
    char *line = NULL;
    size_t len = 0;
    char **actual;
    int actual_count = 0;

    /* Expected translation */
    char *expected[] = {"0x3000\n",
			"0x3050\n",
			"0xE3FE\n",
			"0x6240\n",
			"0x6440\n",
			"0xD698\n",
			"0x3640\n",
			"0x3441\n",
			"0x1262\n",
			"0x103F\n",
			"0x03F9\n",
			"0xF025\n"};


    /* Open/create files for reading ASM code and writing HEX code */
    in = fopen("testCases/shuffle.asm", "r");

    /* Opening <*.hex> in writing and reading mode for rewinding later */
    out = fopen("shuffle.hex", "w+");

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
    ck_assert_int_eq(actual_count, SHUFFLE_ASM_LINES_);
    ck_assert_str_eq(expected[0], actual[0]);
    ck_assert_str_eq(expected[1], actual[1]);
    ck_assert_str_eq(expected[2], actual[2]);
    ck_assert_str_eq(expected[3], actual[3]);
    ck_assert_str_eq(expected[4], actual[4]);
    ck_assert_str_eq(expected[5], actual[5]);
    ck_assert_str_eq(expected[6], actual[6]);
    ck_assert_str_eq(expected[7], actual[7]);
    ck_assert_str_eq(expected[8], actual[8]);
    ck_assert_str_eq(expected[9], actual[9]);
    ck_assert_str_eq(expected[10], actual[10]);
    ck_assert_str_eq(expected[11], actual[11]);
    
    /* Free memory for char* contained within actual */
    for(int j = 0; j < actual_count; j++)
      free(actual[j]);

    /* Free memory of char** */
    free(actual);
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
    Suite *s1 = suite_create("Test Assembler");

    /* Create test cases to add to test suite */
    TCase *tc1_0 = tcase_create("Prog0");
    TCase *tc1_1 = tcase_create("Prog1");
    TCase *tc1_2 = tcase_create("Prog2");
    TCase *tc1_3 = tcase_create("Prog3");
    TCase *tc1_4 = tcase_create("Prog4");
    TCase *tc1_6 = tcase_create("Prog6");
    TCase *tc1_7 = tcase_create("Prog7");
    TCase *tc1_9 = tcase_create("Prog9");
    TCase *tc1_shuffle = tcase_create("Shuffle");

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
    suite_add_tcase(s1, tc1_6);
    suite_add_tcase(s1, tc1_7);
    suite_add_tcase(s1, tc1_9);
    suite_add_tcase(s1, tc1_shuffle);

    /* Add test methods to test cases */
    tcase_add_test(tc1_0, testAssembler_prog0_);
    tcase_add_test(tc1_1, testAssembler_prog1_);
    tcase_add_test(tc1_2, testAssembler_prog2_);
    tcase_add_test(tc1_3, testAssembler_prog3_);
    tcase_add_test(tc1_4, testAssembler_prog4_);
    tcase_add_test(tc1_6, testAssembler_prog6_);
    tcase_add_test(tc1_7, testAssembler_prog7_);
    tcase_add_test(tc1_9, testAssembler_prog9_);
    tcase_add_test(tc1_shuffle, testAssembler_shuffle_);
    
    /* Run all test cases in test suite*/
    srunner_run_all(sr, CK_ENV);

    /* Get number of pass-fails */
    nf = srunner_ntests_failed(sr);

    /* Free memory allocated to test suite */
    srunner_free(sr);

    /* Exit with code 0 if all tests passed*/
    return nf == 0 ? 0 : 1;
}



