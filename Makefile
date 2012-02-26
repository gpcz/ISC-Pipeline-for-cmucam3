# Makefile for a cc3 project


# C files to compile
CSOURCES=main.c ISC_util_assert.c ISC_in_cmucam.c ISC_util_rowqueue.c ISC_util_imagecontext.c ISC_out_histogram.c ISC_util_common.c

# header files
INCLUDES=ISC_util_assert.h ISC_in_cmucam.h ISC_util_rowqueue.h ISC_util_imagecontext.h ISC_out_histogram.h ISC_util_common.h

# header files
LIBS=jpeg-6b zlib

# the name of this project
PROJECT=iscpipeline


# the makefile is useless without the next line!
include ../../include/common.mk
