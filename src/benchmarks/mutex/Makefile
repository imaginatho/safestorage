TARGET=$(BINDIR)/mutex_benchmark

MODULES=mutex_benchmark.o

include ../../Makefile.defaults

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) $(MODULES) -o $@ -L$(LIBDIR) -lsafestorage
	

