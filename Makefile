CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c11
DEPS=add.h subtract.h

maths_prog: maths_prog.c libmaths.so
	$(CC) $(CFLAGS) -L ./ -l maths -o $@ $<

libmaths.so: add.o subtract.o
	$(CC) $(CFLAGS) -shared -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

clean:
	rm *.o *.so maths_prog