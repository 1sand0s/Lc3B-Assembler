## Process this file with automake to produce Makefile.in

TESTS = testAssembler
check_PROGRAMS = testAssembler
testAssembler_SOURCES = testAssembler.c $(top_builddir)/src/Lc3b-Assembler.h
testAssembler_CFLAGS = @CHECK_CFLAGS@
testAssembler_LDADD = $(top_builddir)/src/libAssembler.la @CHECK_LIBS@
