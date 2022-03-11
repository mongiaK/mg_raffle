target:=outfile

CXX:=clang++

INCS=-I ../../utils
CFLAGS=-g -Wall -O0 -std=c++11
LIBS=
OBJ_DIR=obj

DIRS := $(shell find . -maxdepth 5 -type d)
VPATH = ${DIRS}

SRC=$(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
OBJ=$(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%.o,$(SRC)))

all: prepare $(target)

prepare:
	@mkdir -p ${OBJ_DIR}

$(target): ${OBJ}
	${CXX} ${CFLAGS} ${INCS} $^ -o $@ ${LIBS}

$(OBJ_DIR)/%.o: %.cpp
	${CXX} ${CFLAGS} ${INCS} -c $< -o $@

.PHONY: clean

clean:
	@rm -rf ${OBJ_DIR} ${target}
