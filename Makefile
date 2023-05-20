#CFLAGS = -O2

all:
	$(CC) $(CFLAGS) -c *.c
	$(CC) $(CFLAGS) -o main *.o -lm
clean:
	rm -f main
	rm -f *.o

