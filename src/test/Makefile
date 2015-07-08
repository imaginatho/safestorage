TARGET=safe_test

include ../Makefile.defaults

$(TARGET): $(MODULES)
	$(CXX) $(CXXFLAGS) $(MODULES) -o $@ -L$(LIBDIR) -lsafestorage