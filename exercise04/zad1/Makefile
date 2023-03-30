CC = gcc 
CFLAGS = -Wall -g -O3 
HEADER_FILES = $(wildcard *.h)
TARGET_1 = signal_inheritance.exe
TARGET_2 = exec.exe
TARGET = $(TARGET_1) $(TARGET_2)
OBJ_FILE_1 = $(TARGET_1:.exe=.o)
OBJ_FILE_2 = $(TARGET_2:.exe=.o)
OBJ_FILES = $(wildcard *.o)
RESULT = result.txt

.PHONY: all clean clean_obj compile forkTest execTest test clean_compile

all: compile
compile: $(TARGET_1) $(TARGET_2) clean_obj 

$(TARGET_1): $(OBJ_FILE_1)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_2): $(OBJ_FILE_2)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_FILE_1) : $(TARGET_1:.exe=.c) $(HEADER_FILES)
	$(CC) $(CFLAGS) -c $<

$(OBJ_FILE_2) : $(TARGET_2:.exe=.c) $(HEADER_FILES)
	$(CC) $(CFLAGS) -c $<

test: forkTest execTest


forkTest: clean compile
	echo ""	> $(RESULT:.txt=_fork.txt)
	
	echo ">>> Ignore:" >> $(RESULT:.txt=_fork.txt)
	./$(TARGET_1) ignore fork >> $(RESULT:.txt=_fork.txt)

	echo ">>> Pending:" >> $(RESULT:.txt=_fork.txt)
	./$(TARGET_1) pending fork >> $(RESULT:.txt=_fork.txt)

	echo ">>> Mask:" >> $(RESULT:.txt=_fork.txt)
	./$(TARGET_1) mask fork >> $(RESULT:.txt=_fork.txt)

	echo ">>> Handler:" >> $(RESULT:.txt=_fork.txt)
	./$(TARGET_1) handler fork >> $(RESULT:.txt=_fork.txt)

execTest: clean compile
	echo ""	> $(RESULT:.txt=_exec.txt)
	
	echo ">>> Ignore:" >> $(RESULT:.txt=_exec.txt)
	./$(TARGET_1) ignore exec >> $(RESULT:.txt=_exec.txt)

	echo ">>> Pending:" >> $(RESULT:.txt=_exec.txt)
	./$(TARGET_1) pending exec >> $(RESULT:.txt=_exec.txt)

	echo ">>> Mask:" >> $(RESULT:.txt=_exec.txt)
	./$(TARGET_1) mask exec >> $(RESULT:.txt=_exec.txt)

	echo ">>> Handler:" >> $(RESULT:.txt=_exec.txt)
	./$(TARGET_1) handler exec >> $(RESULT:.txt=_exec.txt)

clean_obj:
	rm -f $(OBJ_FILES)

clean_compile:
	rm -f $(OBJ_FILES) $(TARGET)

clean:
	rm -f $(OBJ_FILES) $(TARGET) *.exe $(RESULT:.txt=_fork.txt) $(RESULT:.txt=_exec.txt)


