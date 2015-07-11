TARGET=$(BINDIR)/listener_example

MODULES=listener_example.o

include ../../Makefile.defaults

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) $(MODULES) -o $@ -L$(LIBDIR) -lsafestorage
	

