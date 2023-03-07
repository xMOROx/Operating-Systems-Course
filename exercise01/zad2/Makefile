CC=gcc
CFLAGS=-Wall
LIBRARY_FOLDER=../zad1


all: cleanLib generateLib replForLibraryStatic.exe replForLibraryShared.exe replForLibraryDynamic.exe

generateLib:
	make -C $(LIBRARY_FOLDER)

replForLibraryStatic.exe: replForLibrary.c $(LIBRARY_FOLDER)/libwcCountStatic.a
	$(CC) $(CFLAGS) $< -o $@ -L$(LIBRARY_FOLDER) -lwcCountStatic

replForLibraryShared.exe: replForLibrary.c $(LIBRARY_FOLDER)/libwcCountDynamic.so
	$(CC) $(CFLAGS) $< -o $@ -L$(LIBRARY_FOLDER) -lwcCountDynamic

replForLibraryDynamic.exe: replForLibraryDynamic.c $(LIBRARY_FOLDER)/libwcCountDynamic.so
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.a *.so *.exe
cleanDynamic:
	rm -f *.so *.o
cleanStatic:
	rm -f *.a *.o
cleanLib:
	make -C $(LIBRARY_FOLDER) clean
