TARGET=$(BINDIR)/read_example

MODULES=read_example.o

include ../../Makefile.defaults

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) $(MODULES) -o $@ -L$(LIBDIR) -lsafestorage
	

