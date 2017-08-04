CXX=g++
CPPFLAGS=-g -std=c++14 -O2 -march=native -I /data/opt/brew/include/eigen3/
OUTDIR=build
root=.

all: alsh alsh_bucketing l2lsh l2lsh_bucketing linear_scan

alsh: $(root)/mips/alsh.cpp
	$(CXX) $(CPPFLAGS) $(root)/mips/alsh.cpp -o $(OUTDIR)/alsh

alsh_bucketing: $(root)/mips/alsh_bucketing.cpp
	$(CXX) $(CPPFLAGS) $(root)/mips/alsh_bucketing.cpp -o $(OUTDIR)/alsh_bucketing

l2lsh: $(root)/mips/l2lsh.cpp
	$(CXX) $(CPPFLAGS) $(root)/mips/l2lsh.cpp -o $(OUTDIR)/l2lsh

l2lsh_bucketing: $(root)/mips/l2lsh_bucketing.cpp
	$(CXX) $(CPPFLAGS) $(root)/mips/l2lsh_bucketing.cpp -o $(OUTDIR)/l2lsh_bucketing

linear_scan: $(root)/mips/linear_scan.cpp
	$(CXX) $(CPPFLAGS) $(root)/mips/linear_scan.cpp -o $(OUTDIR)/linear_scan
