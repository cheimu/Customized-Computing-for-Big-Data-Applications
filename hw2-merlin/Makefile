# Copyright (C) 2017 Falcon Computing Solutions, Inc. - All rights reserved.
#
# Choose target FPGA platform & vendor
VENDOR=XILINX
DEVICE=xilinx_aws-vu9p-f1_dynamic_5_0

# Host Code Compilation settings
HOST_SRC_FILES=./src/digitrec_host.cpp ./src/util.cpp

# Executable names and arguments
EXE=test
ACC_EXE=test_acc
# Testing mode
EXE_ARGS= ./data

CXX=g++
CXX_INC_DIRS=-I ./ -I $(MACH_COMMON_DIR)
CXX_FLAGS+= $(CXX_INC_DIRS) -Wall -O3 -std=c++11
ifeq ($(VENDOR),XILINX)
CXX_FLAGS +=-lstdc++ -L$(XILINX_SDX)/lib/lnx64.o
endif

# Accelerated Kernel settings
KERNEL_NAME=digitrec_kernel
KERNEL_SRC_FILES=./src/digitrec.cpp 
KERNEL_INC_DIR=$(CXX_INC_DIRS)

# MerlinCC Options
CMP_OPT=
LNK_OPT=

MCC_COMMON_DIR=${HOME}/Examples/common/
include $(MCC_COMMON_DIR)/mcc_common.mk
