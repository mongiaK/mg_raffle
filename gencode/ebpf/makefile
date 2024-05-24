ARCH ?= $(shell uname -m | sed 's/x86_64/x86/' \
			 | sed 's/arm.*/arm/' \
			 | sed 's/aarch64/arm64/' \
			 | sed 's/ppc64le/powerpc/' \
			 | sed 's/mips.*/mips/' \
			 | sed 's/riscv64/riscv/' \
			 | sed 's/loongarch64/loongarch/')

all: prepare hello

prepare:
	clang -O2 -g -Wall -D__TARGET_ARCH_$(ARCH) -I./root/usr/include -target bpf -c prog_kern.c -o prog_kern.o

hello:
	clang -O2 -g -Wall -std=c++17 -I./root/usr/include -L./root/usr/lib64 prog_user.cpp -lbpf -lelf -lz -o main

call:
	g++ call.cpp -o call

clean:
	@rm prog_kern.o main
