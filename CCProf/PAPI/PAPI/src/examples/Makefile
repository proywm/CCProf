PAPIINC = ..
PAPILIB = ../libpapi.a ../libpapi.so
CC = gcc
CFLAGS += -I$(PAPIINC)
OS = $(shell uname)
CFLAGS += -Wall -fPIC

TARGETS_NTHD = PAPI_set_domain sprofile multiplex PAPI_state PAPI_reset PAPI_profil PAPI_perror PAPI_get_virt_cyc PAPI_get_real_cyc PAPI_get_opt PAPI_hw_info PAPI_get_executable_info PAPI_ipc PAPI_flops PAPI_flips PAPI_overflow PAPI_add_remove_event high_level PAPI_add_remove_events PAPI_overflow_libmonitor

TARGETS_PTHREAD = locks_pthreads overflow_pthreads 

ifeq ($(OS), SunOS)
   LDFLAGS =  $(PAPILIB) -lcpc
   LDFLAGS_PTHREAD =  $(PAPILIB) -lpthread -lcpc
   TARGETS = $(TARGETS_NTHD) $(TARGETS_PTHREAD) 
else 
 ifeq ($(OS), AIX)
   CC = xlc
   LDFLAGS =  $(PAPILIB) -lpmapi
   LDFLAGS_PTHREAD =  $(PAPILIB) -lpthread -lpmapi
   TARGETS = $(TARGETS_NTHD) $(TARGETS_PTHREAD) 
 else
    ifeq ($(OS), OSF1)
        LDFLAGS =  $(PAPILIB) -lrt
        LDFLAGS_PTHREAD =  $(PAPILIB) -lpthread -lrt
        TARGETS = $(TARGETS_NTHD)  
    else
        ifeq ($(OS), Linux)
             TARGETS = $(TARGETS_NTHD) $(TARGETS_PTHREAD) 
        else
             TARGETS = $(TARGETS_NTHD)  
        endif
        LDFLAGS =  $(PAPILIB) -lpthread -lrt
        LDFLAGS_PTHREAD =  $(PAPILIB) -lpthread -lrt
    endif
 endif
endif

all: $(TARGETS) 

$(TARGETS_NTHD):  %:%.o 
	$(CC) -o $@ $(CFLAGS) $^  $(LDFLAGS)

$(TARGETS_PTHREAD):  %:%.o 
	$(CC) -o $@ $(CFLAGS) $^  $(LDFLAGS_PTHREAD)
clean:
	$(RM) *.o $(TARGETS) 

