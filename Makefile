CFLAGS += -Wall -Wextra -Werror
LDFLAGS += -lm -lpthread
all: fractal-gen symlinks

symlinks: fractal-gen
	ln -sf $< mandelbrot-gen
	ln -sf $< burning-ship-gen

fractal-gen: fractal-gen.o \
             algorithms/mandelbrot.o \
             algorithms/burning-ship.o \


clean: clean-object
	rm fractal-gen \
	   mandelbrot-gen \
	   burning-ship-gen \
	   -f

clean-object:
	rm -fv *.o **/*.o

.PHONY: all clean clean-object symlinks
