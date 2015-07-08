TARGET=$(BINDIR)/write_example

MODULES=write_example.o

include ../../Makefile.defaults

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) $(MODULES) -o $@ -L$(LIBDIR) -lsafestorage
	

