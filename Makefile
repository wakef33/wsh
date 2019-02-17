CC=gcc

wsh: src/wsh.c
	$(CC) $< -o bin/wsh

clean:
	rm -f bin/wsh
