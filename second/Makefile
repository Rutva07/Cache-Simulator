all: second

second: second.c
	gcc -Wall -Werror -fsanitize=undefined,address -g second.c -o second

clean:
	rm -rf second
