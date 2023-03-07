CC=gcc 
CFLAGS=-c -Wall
LIBRARY_FOLDER=../zad1
SOURCE_FOLDER=../zad2
SOURCE_CODE=replForLibrary
LIB_PREFIX=libwcCount

FILE_1=../zad2/replForLibrary.c
FILE_2=../zad2/replForLibraryDynamic.c
FILE_3=../zad1/wcCount

all: generatePrograms 

libPrepare: libWcCount libwcCountStatic libwcCountDynamic
compileSource: replForLibraryStatic replForLibraryShared replForLibraryDynamic

libAndCompile: libPrepare compileSource

generatePrograms:
	make libAndCompile SUFFIX=O 
	make libAndCompile SUFFIX=O1
	make libAndCompile SUFFIX=O2
	make libAndCompile SUFFIX=O3
	make libAndCompile SUFFIX=Os

libWcCount:$(LIBRARY_FOLDER)/wcCount.c
	$(CC) $(CFLAGS) -$(SUFFIX) $(LIBRARY_FOLDER)/$< -o wcCountStatic_$(SUFFIX).o #static library
	$(CC) $(CFLAGS) -$(SUFFIX) -fPIC $(LIBRARY_FOLDER)/$< -o wcCountDynamic_$(SUFFIX).o #shared library

libwcCountStatic:wcCountStatic_$(SUFFIX).o
	ar rsc $@_$(SUFFIX).a $<

libwcCountDynamic:wcCountDynamic_$(SUFFIX).o
	$(CC) -$(SUFFIX) -Wall -shared -fPIC -o $@_$(SUFFIX).so $<

replForLibraryStatic: $(SOURCE_FOLDER)/$(SOURCE_CODE).c $(LIB_PREFIX)Static_$(SUFFIX).a
	$(CC) -Wall -$(SUFFIX) $< -o $@_$(SUFFIX).exe -L. -lwcCountStatic_$(SUFFIX)

replForLibraryShared: $(SOURCE_FOLDER)/$(SOURCE_CODE).c $(LIB_PREFIX)Dynamic_$(SUFFIX).so
	$(CC) -Wall -$(SUFFIX) $< -o $@_$(SUFFIX).exe -L. -lwcCountDynamic_$(SUFFIX)

replForLibraryDynamic: $(SOURCE_FOLDER)/$(SOURCE_CODE)Dynamic.c $(LIB_PREFIX)Dynamic_$(SUFFIX).so
	$(CC) -Wall -$(SUFFIX) $< -o $@_$(SUFFIX).exe -D $(SUFFIX)

test: 
	make allTest SUFFIX=O
	make allTest SUFFIX=O1
	make allTest SUFFIX=O2
	make allTest SUFFIX=O3
	make allTest SUFFIX=Os

allTest: testStatic testShared testDynamic

testStatic: 
	echo -e "init 100 \n count $(FILE_1) \n count $(FILE_2) \n count $(FILE_3).c \n count $(FILE_3).h \n show 0 \n show 1 \n show 2 \n show 3 \n delete index 3 \n delete index 2 \n delete index 1 \n delete index 0 \n destroy \n exit \n" | ./replForLibraryStatic_$(SUFFIX).exe > results_static_$(SUFFIX).txt
testShared:
	echo -e "init 100 \n count $(FILE_1) \n count $(FILE_2) \n count $(FILE_3).c \n count $(FILE_3).h \n show 0 \n show 1 \n show 2 \n show 3 \n delete index 3 \n delete index 2 \n delete index 1 \n delete index 0 \n destroy \n exit \n" | ./replForLibraryShared_$(SUFFIX).exe > results_shared_$(SUFFIX).txt
testDynamic: 
	echo -e "init 100 \n count $(FILE_1) \n count $(FILE_2) \n count $(FILE_3).c \n count $(FILE_3).h \n show 0 \n show 1 \n show 2 \n show 3 \n delete index 3 \n delete index 2 \n delete index 1 \n delete index 0 \n destroy \n exit \n" | ./replForLibraryDynamic_$(SUFFIX).exe > results_dynamic_$(SUFFIX).txt

clean:
	rm -f *.o *.a *.so *.exe