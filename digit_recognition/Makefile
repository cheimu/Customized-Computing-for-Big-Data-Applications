#==========================================================================
# Makefile
#==========================================================================
# @brief: A makefile the compiles and runs the digitrec program
#
# @desc: 1. Enter "make" to compile & execute the digitrec program
#        2. Enter "make clean" to clean up the directory (before submission)


# Extract Vivado HLS include path
VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include

COMMON_REPO := ../../..

# wide Memory Access Application
include $(COMMON_REPO)/utility/boards.mk
include $(COMMON_REPO)/libs/xcl2/xcl2.mk
include $(COMMON_REPO)/libs/opencl/opencl.mk

# dot product Host Application
digitrec_host_SRCS=./digitrec_host.cpp ./util.cpp $(xcl2_SRCS)
digitrec_host_HDRS=$(xcl2_HDRS)
digitrec_host_CXXFLAGS=-I./ $(xcl2_CXXFLAGS) $(opencl_CXXFLAGS) -I${VHLS_INC} -DK_CONST=3
digitrec_host_LDFLAGS=$(opencl_LDFLAGS)

# dot product Kernels
digitrec_kernel_SRCS=./digitrec.cpp
digitrec_kernel_CLFLAGS=-k digitrec_kernel


EXES=digitrec_host
XCLBINS=digitrec_kernel

XOS=digitrec_kernel

digitrec_kernel_XOS=digitrec_kernel

# check
check_EXE=digitrec_host ./data
check_XCLBINS=digitrec_kernel

CHECKS=check

include $(COMMON_REPO)/utility/rules.mk

