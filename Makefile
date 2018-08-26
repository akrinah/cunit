OPT_DEBUG = -O0 -g -Wall
OPT_RELEASE = -O3

OPT = ${OPT_DEBUG}
#OPT = ${OPT_RELEASE}


.PHONY: all
all: lib exe


.PHONY: run
run: exe
	@echo "--------------------------------------------------------------------------------"
	@bin/main


.PHONY: lib
lib: lib/libcunit.a


.PHONY: exe
exe: bin/main


.PHONY: clean
clean:
	rm -rf temp/
	rm -rf lib/
	rm -rf bin/


lib/libcunit.a:
	mkdir -p temp/
	mkdir -p lib/
	clang ${OPT} -o temp/cunit.o -Iinclude/ -c src/cunit.c
	ar -rcs lib/libcunit.a temp/cunit.o
	rm -rf temp/


bin/main: lib
	mkdir -p bin/
	clang ${OPT} -o bin/main -Iinclude/ -Llib/ -lcunit src/*.c
