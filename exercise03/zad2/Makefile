CC=	gcc 
CFLAGS=	-g -Wall -Werror -Wextra -pedantic  -O3
FILE= exec_ls.c
TEST1_DIR= ./test_dir 
TEST2_DIR= ~ 
TEST3_DIR= /lib/
OUTPUT_FILE= output_test.txt

all: build

build: $(FILE)
	$(CC) $(CFLAGS) $< -o $(FILE:.c=.exe)

test: clean build 
	./$(FILE:.c=.exe) $(TEST1_DIR) > $(OUTPUT_FILE:.txt=1.txt)
	./$(FILE:.c=.exe) $(TEST2_DIR) > $(OUTPUT_FILE:.txt=2.txt)
	./$(FILE:.c=.exe) $(TEST3_DIR) > $(OUTPUT_FILE:.txt=3.txt)

clean_build: 
	rm -f *.exe

clean: 
	rm -f *.exe *.txt 
