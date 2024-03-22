all: Lab1

CXX = g++
LIB_DIR = /uhome/chome/2024EDA/2024EDA011/HW1/cudd-3.0.0
FLAGS = -lm -O3

Lab1: main.cpp
	$(CXX) -I $(LIB_DIR)/cudd -I $(LIB_DIR)/util -I . main.cpp $(LIB_DIR)/cudd/.libs/libcudd.a $(FLAGS) -o Lab1

clean:
	rm -rf *.o Lab1
	
# LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3

# Lab1: main.o
# 	$(CXX) $(LINKFLAGS) main.o -o Lab1

# main.o: main.cpp
# 	$(CXX) $(LINKFLAGS) -DCOMPILETIME="\"`date`\"" main.cpp -c

# parser.o: parser.cpp parser.h
# 	$(CXX) $(LINKFLAGS) parser.cpp -c

# clean:
# 	rm -rf *.o Lab1






