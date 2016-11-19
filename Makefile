CXX=g++
CXXFLAGS=-Wall

all: test_lcs test_lcs2
clean:
	rm -f test_lcs test_lcs2
distclean: clean
	rm -f a.out *.core *~

test_lcs: test_lcs.cc lcs.h
	$(CXX) -o test_lcs test_lcs.cc
test_lcs2: test_lcs2.cc lcs.h
	$(CXX) -o test_lcs2 test_lcs2.cc
