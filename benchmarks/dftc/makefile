#===============================================================================
# Copyright 2001-2016 Intel Corporation All Rights Reserved.
#
# The source code,  information  and material  ("Material") contained  herein is
# owned by Intel Corporation or its  suppliers or licensors,  and  title to such
# Material remains with Intel  Corporation or its  suppliers or  licensors.  The
# Material  contains  proprietary  information  of  Intel or  its suppliers  and
# licensors.  The Material is protected by  worldwide copyright  laws and treaty
# provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
# modified, published,  uploaded, posted, transmitted,  distributed or disclosed
# in any way without Intel's prior express written permission.  No license under
# any patent,  copyright or other  intellectual property rights  in the Material
# is granted to  or  conferred  upon  you,  either   expressly,  by implication,
# inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
# property rights must be express and approved by Intel in writing.
#
# Unless otherwise agreed by Intel in writing,  you may not remove or alter this
# notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
# suppliers or licensors in any way.
#===============================================================================

## Content:
##      Build and run Intel(R) Math Kernel Library C-style DFT examples.
##*****************************************************************************

help:
	@echo "To run DFTC examples:"
	@echo "  make {libia32|soia32|libintel64|sointel64}"
	@echo "       [compiler=<name>] [interface=<name>] [threading=<name>]"
	@echo "       [parallel=<name>] [omp=<name>] [function=<name>]"
	@echo
	@echo "To get report on run examples:"
	@echo "  make report"
	@echo
	@echo "To clean results:"
	@echo "  make clean"
	@echo
	@echo "To get help just run make or:"
	@echo "  make help"
	@echo
	@echo "Main options:"
	@echo "  targets lib%   use static linkage"
	@echo "  targets so%    use dynamic linkage"
	@echo
	@echo "  compiler=<name> selects the compiler to build the examples:"
	@echo "      compiler=gnu   - GNU gcc"
	@echo "      compiler=pgi   - PGI pgcc. Available only for intel64."
	@echo "      compiler=intel - Intel(R) C compiler icc (DEFAULT)"
	@echo
	@echo "  interface=<name> selects kind of MKL_INT type for %intel64 targets:"
	@echo "      interface=lp64  - 32-bit integers (DEFAULT)"
	@echo "      interface=ilp64 - 64-bit integers"
	@echo
	@echo "  threading=<name> selects threading of MKL:"
	@echo "      threading=parallel   - multithreaded version (DEFAULT)"
	@echo "      threading=sequential - sequential version"
	@echo
	@echo "  parallel=<name> selects MKL threading layer for threading=parallel:"
	@echo "      parallel=intel - libmkl_intel_thread"
	@echo "      parallel=gnu   - libmkl_gnu_thread"
	@echo "      parallel=pgi   - libmkl_pgi_thread"
	@echo "      Default value depends on the setting of compiler=<name>"
	@echo
	@echo "  omp=<name> selects OpenMP runtime library for threading=parallel:"
	@echo "      omp=iomp5 - Intel OpenMP runtime"
	@echo "      omp=gomp  - GNU OpenMP runtime"
	@echo "      omp=pgmp  - PGI OpenMP runtime"
	@echo "      Default value depends on the setting of parallel=<name>"
	@echo
	@echo "  function=<name> selects examples to execute"
	@echo "      Default value: all examples listed in file dftc.lst"
	@echo
	@echo "Additional options:"
	@echo "  RES_DIR=<path> defines where to place the results"
	@echo "      Default value: ./_results"
	@echo
	@echo "  MKLROOT=<path> defines alternative MKL root directory"
	@echo "      Default value: ../.."
	@echo
	@echo "  MKLRUNLIB_PATH=<path> defines alternative runtime library directory"
	@echo "      for dynamic linkage."
	@echo '      Default value: $$(MKLROOT)/lib/$$(IA), with $$(IA) defined by the target'
	@echo
	@echo "  LIB_PATH=<path> defines location of OpenMP runtime libraries"
	@echo "      needed by GNU or PGI threading layer. Set this value if"
	@echo "      respective libraries (libgomp, libpgmp) cannot be found in"
	@echo "      LIBRARY_PATH and LD_LIBRARY_PATH"
	@echo
	@echo "  TARGET_ARCH=<flags> defines additional compiler flags"
	@echo "      Refer to the compiler documentation about the architecture specific"
	@echo "      flags. For example, latest Intel compilers need TARGET_ARCH=-mAVX"
	@echo "      to generate Intel(R) AVX instructions."

##-----------------------------------------------------------------------------
## Usage examples:
##
## make libia32 function=basic_dp_complex_dft_1d
##     Build with Intel(R) C compiler and run this example
##     as a 32-bit application with static linkage
##
## make soia32 compiler=gnu
##     Build with GNU C compiler and run all examples as 32-bit applications
##     with dynamic linkage
##
## make libintel64 compiler=gnu
##     Build with GNU C compiler and run all examples as statically linked
##     applications for Intel(R) 64 processor family
##
## make sointel64
##     Build with Intel(R) C compiler and run all examples as
##     dynamically linked applications for Intel(R) 64 processor family
##-----------------------------------------------------------------------------

MY_MAKEFILE := $(MAKEFILE_LIST)

ifeq (,$(RUN)$(IA))
  target := $(firstword $(filter lib% so%,$(MAKECMDGOALS)))
else
  target = $(RUN)$(IA)
endif

MY_WAY := $(findstring target-specific,$(.FEATURES))

ifndef MKLROOT
  MKLROOT = ../..
endif

ifndef MKLRUNLIB_PATH
  MKLRUNLIB_PATH = $(MKLROOT)/lib/$(IA)
endif

include dftc.lst

ifndef RES_DIR
  RES_DIR = _results
endif

ifndef function
  function = $(DFT)
endif

RES = $(addsuffix .res,$(function))

ifeq (,$(filter gnu pgi,$(compiler)))
  override compiler = intel
  override parallel = intel
endif

ifeq ($(compiler)_$(_IA),pgi_ia32)
   $(info ***)
   $(info *** PGI compiler 32-bit support was dropped off starting from MKL 2017.)
   $(info *** Please try >make help)
   $(info ***)
   $(error )
endif

ifeq (,$(filter lp64 ilp64,$(interface)))
  override interface = lp64
endif
ifeq (,$(findstring 32,$(target)))
  iface = $(interface)_
else
  override interface =
endif

ifeq (,$(filter parallel sequential,$(threading)))
  override threading = parallel
endif

ifneq ($(threading),sequential)
_parallel_intel = intel
_parallel_gnu = gnu
_parallel_pgi = pgi

_omp_intel = iomp5
_omp_gnu = iomp5
_omp_pgi = pgmp

parallel = $(_parallel_$(compiler))
omp = $(_omp_$(parallel))
endif

ifeq (,$(filter gomp pgmp,$(omp)))
  override omp = iomp5
endif

ifeq (,$(filter gnu pgi,$(parallel)))
  override parallel = intel
  override omp = iomp5
else
  ifeq ($(compiler)-$(parallel),gnu-gnu)
    ifneq ($(omp),gomp)
      override omp = iomp5
    endif
  else
    ifeq ($(compiler)-$(parallel),pgi-pgi)
      override omp = pgmp
      override MKL_LIBS_LINK = mixed
    else
      override omp = iomp5
    endif
  endif
endif

LOPTS =

ifeq ($(compiler),gnu)
  LOPTS = -Wl,--no-as-needed
  override CC = gcc
  COPTS.ia32 = -m32
  COPTS.intel64 = -m64
  COPTS.DIAG = -Wall -Werror
  IFACE_COMP_PART = gf
else
  ifeq ($(compiler),pgi)
    CC = pgcc
    COPTS.ia32 = -tp px
    COPTS.intel64 = -tp x64
    COPTS.DIAG = 
  else
    CC = icc
    COPTS.DIAG = -Wall -Werror
  endif
  IFACE_COMP_PART = intel
endif

ifeq ($(compiler)-$(parallel),gnu-gnu)
  IFACE_THREADING_PART = gnu
else
  ifeq ($(compiler)-$(parallel),pgi-pgi)
    IFACE_THREADING_PART = pgi
  else
    IFACE_THREADING_PART = intel
  endif
endif

IFACE_LIB.ia32 = mkl_$(IFACE_COMP_PART)
ifeq ($(interface),ilp64)
  COPTS.intel64 += -DMKL_ILP64
  IFACE_LIB.intel64 = mkl_$(IFACE_COMP_PART)_ilp64
else
  IFACE_LIB.intel64 = mkl_$(IFACE_COMP_PART)_lp64
endif
IFACE_LIB = $(IFACE_LIB.$(IA))

COPTS = $(COPTS.$(IA)) $(COPTS.DIAG)

ifeq ($(threading),sequential)
  threadname = $(threading)
  threadlayer = $(threading)
  THREADING_LIB = mkl_sequential
  OMP_LIB =
else
  threadname = $(threading)_$(omp)
  threadlayer = $(parallel)
  THREADING_LIB = mkl_$(IFACE_THREADING_PART)_thread
  ifeq ($(omp),iomp5)
    OMP_LIB = -L$(CMPLR_PATH) -l$(omp)
  else
    ifneq ($(LIB_PATH),)
      OMP_LIB = -L$(LIB_PATH)
    endif
    OMP_LIB += -l$(omp)
  endif
endif

CORE_LIB = mkl_core

ifeq ($(compiler)-$(IFACE_THREADING_PART),pgi-pgi)
  COPTS += -mp -pgf90libs
endif

ifeq ($(compiler),pgi)
  COPTS += -Mnokeepobj
endif

ifeq ($(compiler),intel)
  COPTS += -qopenmp
endif

ifeq ($(compiler),gnu)
  COPTS += -fopenmp
endif

COPTS += -g

MKL_PATH = $(MKLROOT)/lib/$(IA)
CMPLR_PATH = $(MKLROOT)/../compiler/lib/$(IA)

res_dir = $(RES_DIR)/$(compiler)_$(iface)$(threadname)_$(target)

ifdef VERBOSE
  $(info )
  $(info MAKELEVEL=$(MAKELEVEL) target=$(target))
  $(info MKLROOT=$(MKLROOT) RES_DIR=$(RES_DIR))
  $(info compiler=$(compiler) interface=$(interface) threading=$(threading) \
         parallel=$(parallel) omp=$(omp))
endif

##-----------------------------------------------------------------------------
## Rules

.PHONY: libia32 soia32 libintel64 sointel64 \
	lib32 so32 libem64t soem64t \
	help run mkdir clean report \
	FORCE

.SUFFIXES:
.SUFFIXES: .c .o .res

vpath %.c source

# just old targets for compatibility
lib32: libia32
so32: soia32
libem64t: libintel64
soem64t: sointel64

ifeq ($(MY_WAY),target-specific$(RECURSION))

lib%: RUN = lib
so%: RUN = so

%ia32: IA = ia32
%intel64: IA = intel64

ifneq ($(LIB_PATH),)
lib%: RUNENV = LD_LIBRARY_PATH=$(LIB_PATH):$(CMPLR_PATH):$(LD_LIBRARY_PATH)
else
lib%: RUNENV = LD_LIBRARY_PATH=$(CMPLR_PATH):$(LD_LIBRARY_PATH)
endif
lib%: MKL_LIBS = $(MKL_PATH)/lib$(IFACE_LIB).a
lib%: MKL_LIBS += -Wl,--start-group
lib%: MKL_LIBS += $(MKL_PATH)/lib$(THREADING_LIB).a $(MKL_PATH)/lib$(CORE_LIB).a
lib%: MKL_LIBS += -Wl,--end-group

ifneq ($(LIB_PATH),)
so%: RUNENV = LD_LIBRARY_PATH=$(LIB_PATH):$(MKLRUNLIB_PATH):$(CMPLR_PATH):$(LD_LIBRARY_PATH)
else
so%: RUNENV = LD_LIBRARY_PATH=$(MKLRUNLIB_PATH):$(CMPLR_PATH):$(LD_LIBRARY_PATH)
endif
ifneq ($(MKL_LIBS_LINK),mixed)
so%: MKL_LIBS = $(LOPTS) -L$(MKL_PATH) -lmkl_rt
so%: RUNENV += MKL_INTERFACE_LAYER=$(interface) MKL_THREADING_LAYER=$(threadlayer)
else
so%: MKL_LIBS = $(LOPTS) -L$(MKL_PATH) -l$(IFACE_LIB) -l$(THREADING_LIB) -l$(CORE_LIB)
endif

libia32 soia32 libintel64 sointel64: run

else # using recursive MAKE if target-specific targets are unsupported

ifeq ($(RUN),so)
  ifneq ($(LIB_PATH),)
    RUNENV = LD_LIBRARY_PATH=$(LIB_PATH):$(MKLRUNLIB_PATH):$(CMPLR_PATH):$(LD_LIBRARY_PATH)
  else
    RUNENV = LD_LIBRARY_PATH=$(MKLRUNLIB_PATH):$(CMPLR_PATH):$(LD_LIBRARY_PATH)
  endif
  ifneq ($(MKL_LIBS_LINK),mixed)
    MKL_LIBS = $(LOPTS) -L$(MKL_PATH) -lmkl_rt
    RUNENV += MKL_INTERFACE_LAYER=$(interface) MKL_THREADING_LAYER=$(threadlayer)
  else
    MKL_LIBS = $(LOPTS) -L$(MKL_PATH) -l$(IFACE_LIB) -l$(THREADING_LIB) -l$(CORE_LIB)
  endif
else
  ifneq ($(LIB_PATH),)
    RUNENV = LD_LIBRARY_PATH=$(LIB_PATH):$(CMPLR_PATH):$(LD_LIBRARY_PATH)
  else
    RUNENV = LD_LIBRARY_PATH=$(CMPLR_PATH):$(LD_LIBRARY_PATH)
  endif
  MKL_LIBS = $(MKL_PATH)/lib$(IFACE_LIB).a
  MKL_LIBS += -Wl,--start-group
  MKL_LIBS += $(MKL_PATH)/lib$(THREADING_LIB).a $(MKL_PATH)/lib$(CORE_LIB).a
  MKL_LIBS += -Wl,--end-group
endif

libia32:
	$(MAKE) -f $(MY_MAKEFILE) run IA=ia32 RUN=lib

soia32:
	$(MAKE) -f $(MY_MAKEFILE) run IA=ia32 RUN=so

libintel64:
	$(MAKE) -f $(MY_MAKEFILE) run IA=intel64 RUN=lib

sointel64:
	$(MAKE) -f $(MY_MAKEFILE) run IA=intel64 RUN=so

endif

run: mkdir $(RES)

mkdir: FORCE
	@echo
	@echo See results in $(res_dir)
	@echo
	mkdir -p $(res_dir)

%.res: %.c FORCE
	@echo
	# compile $*.c
	$(CC) $(COPTS) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH) \
	  -I$(MKLROOT)/include \
	  $< \
	  $(MKL_LIBS) \
	  $(OMP_LIB) -lpthread -lm -ldl -o $(res_dir)/$*.out
	-rm -f $(res_dir)/$@ # remove res-file
	@echo
	# run $*.out
	$(RUNENV) \
	  $(res_dir)/$*.out > $(res_dir)/$@

report: FORCE
	@echo
	@echo "--------------------------------------------------"
	@echo There are `ls source/*.c | wc -l` \
	  examples in source
	@echo
	@for D in `ls -d $(RES_DIR)/* 2>/dev/null`; do \
	  echo -n "In   $$D	Total run examples:" \
	    "`ls $$D/*.res 2>/dev/null | wc -l`"; \
	  echo -n "	and Passed: "; \
	  cat /dev/null $$D/*.res 2>/dev/null | grep -c PASSED | cat; \
	done
	@echo "--------------------------------------------------"
	@echo

clean: FORCE
	-rm -rf $(RES_DIR) # clean all results

FORCE: ;

##-----------------------------------------------------------------------------
