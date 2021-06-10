CXX:=clang++

INCS=-I ../utils

CFLAGS=-g -Wall -O0 -std=c++11
LIBS=

SRC=${wildcard *.cpp}
OBJ=${patsubst %.cpp,%.o,${SRC}}

.PHONY: clean

all: namespace

namespace: main.o
	${CXX} ${CFLAGS} ${INCS} $^ -o $@ ${LIBS}

%.o:%.cpp
	${CXX} ${CFLAGS} ${INCS} -c $< -o $@

clean:
	@rm -f ${OBJ} namespace
