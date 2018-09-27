COMMON_REPO := ../../../

# wide Memory Access Application
include $(COMMON_REPO)/utility/boards.mk
include $(COMMON_REPO)/libs/xcl2/xcl2.mk
include $(COMMON_REPO)/libs/opencl/opencl.mk

# dot product Host Application
dot_prod_host_SRCS=./src/dot_prod_host.cpp $(xcl2_SRCS)
dot_prod_host_HDRS=$(xcl2_HDRS)
dot_prod_host_CXXFLAGS=-I./src/ $(xcl2_CXXFLAGS) $(opencl_CXXFLAGS)
dot_prod_host_LDFLAGS=$(opencl_LDFLAGS)

# dot product Kernels
dot_prod_kernel_SRCS=./src/dot_prod_kernel.cpp
dot_prod_kernel_CLFLAGS=-k dot_prod_kernel

EXES=dot_prod_host
XCLBINS=dot_prod_kernel

XOS=dot_prod_kernel

dot_prod_kernel_XOS=dot_prod_kernel

# check
check_EXE=dot_prod_host
check_XCLBINS=dot_prod_kernel

CHECKS=check

include $(COMMON_REPO)/utility/rules.mk

