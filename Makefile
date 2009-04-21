CC=gcc
CFLAGS=-Wall -g
BIN=msh
# FILES=$(SRC)/msh.c

# $< the name of the related file that caused the action.
# $* the prefix shared by target and dependent files. 

compile:
	@echo compiling!
	cd src && make

install: compile
	cd src && make install

clean:
	cd src && make clean

test-run:
	@echo testrun: -------------------------
	$(BIN)