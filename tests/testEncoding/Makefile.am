## Process this file with automake to produce Makefile.in

TESTS = testEncoding
check_PROGRAMS = testEncoding
testEncoding_SOURCES = testEncoding.c $(top_builddir)/src/Lc3b-Assembler.h
testEncoding_CFLAGS = @CHECK_CFLAGS@
testEncoding_LDADD = $(top_builddir)/src/libAssembler.la @CHECK_LIBS@
