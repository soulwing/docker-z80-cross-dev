PROG=z80-bin

all:	$(PROG)
	
clean:
	rm -f $(PROG) *.o

$(PROG): $(PROG).o
	$(CC) -o $@ $^

$(PROG).o: $(PROG).c
