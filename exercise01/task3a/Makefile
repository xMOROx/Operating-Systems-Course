FILE_1=../zad2/replForLibrary.c
FILE_2=../zad2/replForLibraryDynamic.c
FILE_3=../zad1/wcCount


all: compilePrograms test

compilePrograms: 
	make -C ../zad2 all 

test: testStatic testShared testDynamic
	
testStatic: 
	echo -e "init 100 \n count $(FILE_1) \n count $(FILE_2) \n count $(FILE_3).c \n count $(FILE_3).h \n show 0 \n show 1 \n show 2 \n show 3 \n delete index 3 \n delete index 2 \n delete index 1 \n delete index 0 \n destroy \n exit \n" | ../zad2/replForLibraryStatic.exe > results_static.txt
testShared:
	echo -e "init 100 \n count $(FILE_1) \n count $(FILE_2) \n count $(FILE_3).c \n count $(FILE_3).h \n show 0 \n show 1 \n show 2 \n show 3 \n delete index 3 \n delete index 2 \n delete index 1 \n delete index 0 \n destroy \n exit \n" | ../zad2/replForLibraryShared.exe > results_shared.txt
testDynamic: 
	echo -e "init 100 \n count $(FILE_1) \n count $(FILE_2) \n count $(FILE_3).c \n count $(FILE_3).h \n show 0 \n show 1 \n show 2 \n show 3 \n delete index 3 \n delete index 2 \n delete index 1 \n delete index 0 \n destroy \n exit \n" | ../zad2/replForLibraryDynamic.exe > results_dynamic.txt

clean: 
	make -C ../zad2 clean
	make -C ../zad1 clean