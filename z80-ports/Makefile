PROG=z80-ports

all:	$(PROG)
	
clean:
	rm -f $(PROG) *.o

$(PROG): $(PROG).o
	$(CC) -o $@ $^

$(PROG).o: $(PROG).c
