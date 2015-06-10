all: fractal-gen symlinks

symlinks: fractal-gen
	ln -sf $< mandelbrot-gen
	ln -sf $< burning-ship-gen
	ln -sf $< burning-ship-lattice-gen
	ln -sf $< tricorn-gen

fractal-gen: fractal-gen.o \
             algorithms/mandelbrot.o \
             algorithms/burning-ship.o \
             algorithms/burning-ship-lattice.o \
             algorithms/tricorn.o

	$(CC) -o $@ $^ -lm -lpthread

%.o: %.c
	$(CC) -c -o $@ $< -Wall -Wextra -Werror

.PHONY: all clean clean-object symlinks
clean: clean-object
	rm fractal-gen \
	   mandelbrot-gen \
	   burning-ship-gen \
	   burning-ship-lattice-gen \
	   tricorn-gen \
	   -f

clean-object:
	rm -fv *.o **/*.o
