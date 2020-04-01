PROG=z80-console

all:	$(PROG)
	
clean:
	rm -f $(PROG) *.o

$(PROG): console.o
	$(CC) -o $@ $^

console.o: console.c
