include config.mk

all: clean test benchmark-gcc benchmark-clang

run_test:
	./test

valgrind_test:
	valgrind ./test

run_benchmark:
	./benchmark

test: test.cpp vector.h
	g++ -g $(CXXFLAGS) test.cpp -o test

benchmark-gcc: benchmark.cpp vector.h
	g++ $(CXXFLAGS) $(CXXFLAGSOPT) benchmark.cpp -lbenchmark `llvm-config --ldflags` -lLLVM -o benchmark-gcc

benchmark-clang: benchmark.cpp vector.h
	clang++ $(CXXFLAGS) $(CXXFLAGSOPT) benchmark.cpp -lbenchmark `llvm-config --ldflags` -lLLVM -o benchmark-clang

clean:
	rm -f test benchmark-gcc benchmark-clang