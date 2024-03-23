all: Lab1

CXX = g++
LIB_DIR = /uhome/chome/2024EDA/2024EDA011/HW1/cudd-3.0.0
HEADER_FLAG = -I header/
FLAGS = -lm -O3

Lab1: main.o parser.o robdd.o
	$(CXX) $(FLAGS) -I $(LIB_DIR)/cudd -I $(LIB_DIR)/util -I . main.o parser.o robdd.o $(LIB_DIR)/cudd/.libs/libcudd.a -o Lab1

main.o: src/main.cpp
	$(CXX) $(FLAGS) $(HEADER_FLAG) -I $(LIB_DIR)/cudd -I $(LIB_DIR)/util -I . -c src/main.cpp -o main.o

parser.o: src/parser.cpp
	$(CXX) $(FLAGS) $(HEADER_FLAG) -c src/parser.cpp -o parser.o

robdd.o: src/robdd.cpp
	$(CXX) $(FLAGS) $(HEADER_FLAG) -I $(LIB_DIR)/cudd -I $(LIB_DIR)/util -I . -c src/robdd.cpp -o robdd.o

clean:
	rm -rf *.o Lab1