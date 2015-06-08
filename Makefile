all: fractal-gen symlinks

symlinks: fractal-gen
	ln -sf $< mbrot-gen
	ln -sf $< bship-gen

fractal-gen: fractal-gen.o mbrot.o bship.o
	$(CC) -o $@ $^ -lm -lpthread

%.o: %.c
	$(CC) -c -o $@ $< -Wall -Wextra -Werror


.PHONY: all clean symlinks
clean:
	rm fractal-gen \
	   mbrot-gen   \
	   bship-gen   \
	   *.o \
	   -f
