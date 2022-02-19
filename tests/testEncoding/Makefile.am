## Process this file with automake to produce Makefile.in

TESTS = testORIG testFILL testEND testNOP testTRAP testHALT testADD
check_PROGRAMS = testORIG testFILL testEND testNOP testTRAP testHALT testADD

# Run tests for encoding ORIG 
testORIG_SOURCES = testORIG.c $(top_builddir)/src/Lc3b-Assembler.h
testORIG_CFLAGS = @CHECK_CFLAGS@
testORIG_LDADD = $(top_builddir)/src/libAssembler.la @CHECK_LIBS@

# Run tests for encoding FILL
testFILL_SOURCES = testFILL.c $(top_builddir)/src/Lc3b-Assembler.h
testFILL_CFLAGS = @CHECK_CFLAGS@
testFILL_LDADD = $(top_builddir)/src/libAssembler.la @CHECK_LIBS@

# Run tests for encoding END
testEND_SOURCES = testEND.c $(top_builddir)/src/Lc3b-Assembler.h
testEND_CFLAGS = @CHECK_CFLAGS@
testEND_LDADD = $(top_builddir)/src/libAssembler.la @CHECK_LIBS@

# Run tests for encoding NOP
testNOP_SOURCES = testNOP.c $(top_builddir)/src/Lc3b-Assembler.h
testNOP_CFLAGS = @CHECK_CFLAGS@
testNOP_LDADD = $(top_builddir)/src/libAssembler.la @CHECK_LIBS@

# Run tests for encoding TRAP
testTRAP_SOURCES = testTRAP.c $(top_builddir)/src/Lc3b-Assembler.h
testTRAP_CFLAGS = @CHECK_CFLAGS@
testTRAP_LDADD = $(top_builddir)/src/libAssembler.la @CHECK_LIBS@

# Run tests for encoding HALT
testHALT_SOURCES = testHALT.c $(top_builddir)/src/Lc3b-Assembler.h
testHALT_CFLAGS = @CHECK_CFLAGS@
testHALT_LDADD = $(top_builddir)/src/libAssembler.la @CHECK_LIBS@

# Run tests for encoding ADD
testADD_SOURCES = testADD.c $(top_builddir)/src/Lc3b-Assembler.h
testADD_CFLAGS = @CHECK_CFLAGS@
testADD_LDADD = $(top_builddir)/src/libAssembler.la @CHECK_LIBS@
