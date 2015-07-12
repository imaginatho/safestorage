TARGET=$(BINDIR)/replica_example

MODULES=replica_example.o

include ../../Makefile.defaults

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) $(MODULES) -o $@ -L$(LIBDIR) -lsafestorage
	

