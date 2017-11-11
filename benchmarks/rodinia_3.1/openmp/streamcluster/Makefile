PREFIX=${PARSECDIR}/pkgs/kernels/streamcluster/inst/${PARSECPLAT}

TARGET_C = sc_cpu

TARGET_O = sc_omp

ifdef version
  ifeq "$(version)" "parallel"
    CXXFLAGS :=	-g $(CXXFLAGS) -DENABLE_THREADS -pthread
  endif
endif

all: cpu omp

cpu:
	$(CXX) -g $(CXXFLAGS) $(LDFLAGS) streamcluster_original.cpp -o $(TARGET_C) $(LIBS) -DENABLE_THREADS -pthread

omp:
	g++ -g -O0 -fopenmp -o $(TARGET_O) streamcluster_omp.cpp


clean:
	rm -f *.o *~ *.txt sc_cpu sc_omp
	
