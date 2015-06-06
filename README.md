Mandelbrot
==========
This is a tiny program which will output a very basic raw format square image of a mandelbrot set.

Just invoke it like 

	./mbrot-gen 1000 100 2 > image.raw

For a 1000x1000 point image of a mandelbrot set using 100 iterations and an exponent of 2.
For the clich&eacute; set you'll want to keep the exponent at 2.

You'll then want something to turn this megabyte monster into something pretty you can look at.
I've got some pieces of software for viewing live and also processing into PNG tiles, but they need a tidy up before putting on GitHub.

Each point is represented in the output format as a byte.
This byte's value ranges in proportion with the number of iterations it took to decide if the point is in the set or not.
In the aboove, a value of 255 means it took 100 iterations, a value of 127 means 50 iterations, and so on.


Notes
-----

1. This has your CPU over a barrel.
   It spawns as many threads as you have cores, so have fun with that.
   I'm looking at adding an option to override the number of threads.


