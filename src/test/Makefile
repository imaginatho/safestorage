TARGET=$(BINDIR)/safe_test

MODULES=safe_test.o

include ../Makefile.defaults

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) $(MODULES) -o $@ -L$(LIBDIR) -lsafestorage