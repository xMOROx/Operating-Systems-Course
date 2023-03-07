CC=gcc
CFLAGS=-c -Wall


all: cleanLib generateLib replForLibraryStatic.exe replForLibraryShared.exe replForLibraryDynamic.exe

generateLib:
	make -C ../zad1

replForLibraryStatic.exe: replForLibrary.c ../zad1/libwcCountStatic.a
	$(CC) -Wall $< -o $@ -L../zad1 -lwcCountStatic

replForLibraryShared.exe: replForLibrary.c ../zad1/libwcCountDynamic.so
	$(CC) -Wall $< -o $@ -L../zad1 -lwcCountDynamic

replForLibraryDynamic.exe: replForLibraryDynamic.c ../zad1/libwcCountDynamic.so
	$(CC) -Wall $< -o $@

clean:
	rm -f *.o *.a *.so *.exe
cleanDynamic:
	rm -f *.so *.o
cleanStatic:
	rm -f *.a *.o
cleanLib:
	make -C ../zad1 clean
