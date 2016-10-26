CFLAGS += -Wall -Wextra -Werror
LDFLAGS += -lm -lpthread

HEADERS = generator.h algorithms.h fractal-gen.h


all: fractal-gen symlinks

symlinks: fractal-gen
	ln -sf $< mandelbrot-gen
	ln -sf $< burning-ship-gen

fractal-gen: fractal-gen.o \
             algorithms/mandelbrot.o \
             algorithms/burning-ship.o \

*.o: *.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: clean-object
	rm fractal-gen \
	   mandelbrot-gen \
	   burning-ship-gen \
	   -f

clean-object:
	rm -fv *.o **/*.o

.PHONY: all clean clean-object symlinks
