OPT_DEBUG = -O0 -g -Wall
OPT_RELEASE = -O3

#OPT = ${OPT_DEBUG}
OPT = ${OPT_RELEASE}


.PHONY: all
all: lib exe test_exe


.PHONY: clean
clean:
	rm -rf temp/
	rm -rf lib/
	rm -rf bin/


.PHONY: run
run: exe


.PHONY: test
test: test_exe
	@echo ""
	@bin/test
	@echo "--------------------------------------------------------------------------------"


.PHONY: test_memleak
test_memleak: test_exe
	@echo ""
	@valgrind --leak-check=full bin/test > /dev/null
	@echo "--------------------------------------------------------------------------------"


######################################## INTERNAL ########################################


.PHONY: lib
lib: deps
	mkdir -p temp/
	mkdir -p lib/
	clang ${OPT} -c -Iinclude/ -o temp/cunit.o src/cunit.c
	ar -rcs lib/libcunit.a temp/cunit.o
	rm -rf temp/


.PHONY: exe
exe: deps lib


.PHONY: deps
deps:


.PHONY: test_exe
test_exe: lib test_deps
	mkdir -p bin/
	clang ${OPT} -Iinclude/ -Llib/ -o bin/test src/*.c -lcunit


.PHONY: test_deps
test_deps:
