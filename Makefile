OPT_DEBUG = -O0 -g -Wall
OPT_RELEASE = -O3

#OPT = ${OPT_DEBUG}
OPT = ${OPT_RELEASE}


.PHONY: all
all: lib exe


.PHONY: run
run: exe
	@bin/main
	@echo "--------------------------------------------------------------------------------"


.PHONY: lib
lib:
	mkdir -p temp/
	mkdir -p lib/
	clang ${OPT} -c -Iinclude/ -o temp/cunit.o src/cunit.c
	ar -rcs lib/libcunit.a temp/cunit.o
	rm -rf temp/


.PHONY: exe
exe: lib
	mkdir -p bin/
	clang ${OPT} -Iinclude/ -Llib/ -o bin/main src/*.c -lcunit


.PHONY: clean
clean:
	rm -rf temp/
	rm -rf lib/
	rm -rf bin/
