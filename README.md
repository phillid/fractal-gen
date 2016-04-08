Fractal Gen
==========
This is a tiny program which will output a binary [PGM](https://wikipedia.org/wiki/Netpbm_format#PGM_example) format square image of a mandelbrot or burning ship set.


Syntax
------

    ./mandelbrot-gen -c size -i max_iterations [-e exponent] \
                     [-c cores] [-t thread_multiplier]
                     [-N cluster-id -T cluster-total] > output.pgm

Also check out the other algorithms like `tricorn-gen` and `burning-ship-gen`.
You might then want to consider using a tool such as ImageMagick to then convert the (large) output file into something more sane like a PNG.
For the clich&eacute; set you'll want to keep the exponent at 2.
For more info on the exponent, read through [Wikipedia's fine article](http://wikipedia.org/wiki/Mandelbrot_set).

Notes
-----

1. By default, this has your CPU over a barrel.
   It spawns as many threads as you have cores, so have fun with that.
   You can specify the number of threads you want it to (see syntax).

2. PGMs are grayscale, so you might want to tint it or apply a pallete of sort.
