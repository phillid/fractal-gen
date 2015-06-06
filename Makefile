all: fractal-gen symlinks

symlinks: fractal-gen
	ln -sf $< mbrot-gen
	ln -sf $< bship-gen

fractal-gen: fractal-gen.c
	$(CC) -o $@ $< -lm -lpthread -Wall -Wextra -Werror


.PHONY: all clean
clean:
	rm fractal-gen \
	   mbrot-gen   \
	   bship-gen   \
	   -f
