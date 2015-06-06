all: mbrot-gen

mbrot-gen: mbrot-gen.c
	$(CC) -o $@ $@.c -lm -lpthread -Wall -Wextra


.PHONY: all clean
clean:
	- rm mbrot-gen -f
