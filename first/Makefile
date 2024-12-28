all: first

first: first.c
	gcc -Wall -Werror -fsanitize=undefined,address -g first.c -o first

clean:
	rm -rf first