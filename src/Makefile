CC=gcc
CFLAGS=-std=c99 -Wall -g
LDFLAGS=
BIN=msh
SOURCES.c= msh.c execute.c parser.c
OBJECTS= $(SOURCES.c:.c=.o)

msh: $(OBJECTS)
	@echo compiling mdu!
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN)

.c.o:
	@echo .c.o wtf
	$(CC) $(CFLAGS) -c $<

install:
	@echo Installing msh
	mv $(BIN) ../bin

clean:
	@echo cleaning mdu!
	rm $(BIN) $(OBJECTS)

test-run:
	@echo testrun: -------------------------
	$(BIN)