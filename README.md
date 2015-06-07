Fractal Gen
==========
This is a tiny program which will output a binary [PGM](https://wikipedia.org/wiki/Netpbm_format#PGM_example) format square image of a mandelbrot or burning ship set.

Just invoke it like 

	./mbrot-gen 1000 100 2 > image.pgm

For a 1000x1000 point image of a mandelbrot set using 100 iterations and an exponent of 2.
For the clich&eacute; set you'll want to keep the exponent at 2.

Notes
-----

1. This has your CPU over a barrel.
   It spawns as many threads as you have cores, so have fun with that.
   I'm looking at adding an option to override the number of threads.

2. PGMs are grayscale, so you might want to tint it or apply a pallete of sort.
