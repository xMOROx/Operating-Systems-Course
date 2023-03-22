CC=	gcc 
CFLAGS=	-g -Wall -Werror -Wextra -pedantic  -O3
FILE= create_child_processes.c 

all: $(FILE)
	$(CC) $(CFLAGS) -o $(FILE:.c=.exe) $<

test: clean  all
	./$(FILE:.c=.exe) 100 > $(FILE:.c=.txt)

clean_build:
	rm -f *.exe

clean:
	rm -f *.exe *.txt
