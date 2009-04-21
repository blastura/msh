CC=gcc
CFLAGS=-Wall -g
LDFLAGS=
BIN=msh

# $< the name of the related file that caused the action.
# $* the prefix shared by target and dependent files. 

msh: msh.o parser.o execute.o
	@echo compiling mdu!
	$(CC) $(CFLAGS) $? -o $@

.c.o:
	@echo wtf
	$(CC) -c $<

install:
	@echo Installing msh
	mv msh ../bin

clean:
	@echo cleaning mdu!
	rm msh


test-run:
	@echo testrun: -------------------------
	$(BIN)