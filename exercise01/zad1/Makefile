CC=gcc
CFLAGS=-c -Wall

all:libWcCount libwcCountStatic.a libwcCountDynamic.so

libWcCount:wcCount.c
	$(CC) $(CFLAGS) $< -o wcCountStatic.o #static library
	$(CC) $(CFLAGS) -fPIC $< -o wcCountDynamic.o #dynamic library

libwcCountStatic.a:wcCountStatic.o
	ar rsc $@ $< 

libwcCountDynamic.so:wcCountDynamic.o
	$(CC) -Wall -shared -fPIC -o $@ $<

clean:
	rm -f *.o *.a *.so
cleanDynamic:
	rm -f *.so *.o
cleanStatic:
	rm -f *.a *.o