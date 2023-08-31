target:=leetcode

CXX:=clang++

INCS=-I/usr/local/include
CFLAGS=-g -Wall -O0 -std=c++11

LIB_INCS=-L//usr/local/lib
STATIC_LIBS=
DYNAMIC_LIBS=

OBJ_DIR=obj

DIRS := $(shell find . -maxdepth 5 -type d)
VPATH = ${DIRS}

SRC=$(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
OBJ=$(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%.o,$(notdir $(SRC))))

all: prepare $(target)

prepare:
	@mkdir -p ${OBJ_DIR}

$(target): ${OBJ}
	${CXX} ${CFLAGS} ${INCS} $^ -o $@ ${LIB_INCS} -Wl,-Bstatic ${STATIC_LIBS} -Wl,-Bdynamic ${DYNAMIC_LIBS}

$(OBJ_DIR)/%.o: %.cpp
	${CXX} ${CFLAGS} ${INCS} -c $< -o $@

.PHONY: clean

clean:
	@rm -rf ${OBJ_DIR} ${target}
