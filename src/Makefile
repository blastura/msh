CC=gcc
CFLAGS=-Wall -g
LDFLAGS=
BIN=msh

# $< the name of the related file that caused the action.
# $* the prefix shared by target and dependent files. 

msh: msh.o execute.o parser.o
	@echo compiling mdu!
	$(CC) $(CFLAGS) $? -o $@

.c.o:
	@echo .c.o wtf
	$(CC) -c $<

install:
	@echo Installing msh
	mv msh ../bin

clean:
	@echo cleaning mdu!
	rm msh *.o


test-run:
	@echo testrun: -------------------------
	$(BIN)