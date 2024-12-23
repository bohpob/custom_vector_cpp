include config.mk

run:
	./test

valgrind:
	valgrind ./test

test: test.cpp vector.h
	g++ -g $(CXXFLAGS) test.cpp -o test
