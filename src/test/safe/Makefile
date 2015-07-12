include ../../Makefile.defaults

TARGET=$(BINDIR)/safe_test

MODULES=safe_test.o

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) $(MODULES) -o $@ -L$(LIBDIR) -lsafestorage