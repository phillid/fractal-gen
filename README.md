# Fractal Gen
This is a tiny program which will output a binary [PGM](https://wikipedia.org/wiki/Netpbm_format#PGM_example) format square image of a mandelbrot or burning ship set.


## Syntax

	./mandelbrot-gen -s size -i max_iterations [-e exponent] \
	                 [-c cores] [-t thread_multiplier]
	                 [-x left -y top] [-z scale]
	                 [-N cluster-id -T cluster-total] > output.pgm

Also check out the other algorithms like `tricorn-gen` and `burning-ship-gen`.
You might then want to consider using a tool such as ImageMagick to then
convert the (large) output file into something more sane like a PNG.
For the cliché set you'll want to keep the exponent at 2.


### Options in detail

The `-s` and `-i` are the only two mandatory arguments. All others will assume
their default values (shown here in brackets) if left unspecified.

Also note that `-N` should always be used together with `-T`.

	-s size              Integer
	                     Height and width of image in pixels
	-s max_iterations    Integer
	                     Maximum number of iterations to use to determine if
	                     number is in the fractal set.
	-e exponent          Float
	                     Exponent to use in the algorithm. (2)
	-c cores             Integer > 0
	                     Number of cores to assume computer has.
	                     (sysconf(_SC_NPROCESSORS_ONLN))
	-t thread_multiplier Float > 0
	                     Coefficient to multiply cores by in order to achieve
	                     count of worker threads to use. (1)
	-x left              position of left of image on the real axis (x+iy)
	-y top               position of top of image on the imaginary axis (x+iy)
	-z scale             width and height of image in units on the cartesian
	                     plane
	-N cluster_id        0 <= Integer < cluster_total
	                     A unique ID used to determine which sections of the
	                     image this instance should work on.
	-T cluster_total     0 < Integer <= size
	                     Total count of worker instances in this cluster.


## Spreading computation load

The workload for an image can be spread across multiple threads and multiple
invocations. This means, for example, that you can parallel the computation
on an image between multiple computers.


### Across threads

When determining how many worker threads to spawn for an invocation,
fractal-gen will do three things:

1. Get the thread count from the `-c` flag
	* Failing this, detect the number of cores available (`_SC_NPROCESSORS_ONLN`)
3. Multiply this by the thread coefficient (`-t` option)

With the default thread coefficient of 1.0, this will normally just spawn
a worker thread for every available core on the system. The worker threads
then divide the image up between themselves by splitting it into columns.
On an invocation with two worker threads, one will be working on the
even-numbered columns and the other will work on the odd-numbered ones.

It may become advantageous to adjust the thread coefficient to a number above
or below unity. At a squeeze, I have had small wall clock gains when running
at a coefficient of, say, 5. On the other hand, you might want to scale the
workers back to a fraction of your available cores if you have other things
to allocate processor time towards.


### Across invocations

Let's say we're generating a quick square mandelbrot 20000 pixels in width and
with 100 iterations max. We want to spread the load between two identical
computers in order to perhaps halve the computation time:

	# On computer #1:
	./mandelbrot -s 20000 -i 100 -T 2 -N 0 > 0.pgm
	# On computer #2:
	./mandelbrot -s 20000 -i 100 -T 2 -N 1 > 1.pgm

At the heart of it, each instance will only calculate half of the image. This
is achieved by (in this case) getting the instance with ID 0 to work on
the odd numbered rows, while the instance with ID 1 will work on the even
numbered rows in between.

This will result in two separate pgm files, each 20000x10000 pixels.
They must be interlaced in order to form the whole desired image.
This extends to any number of cluster members less than or equal to the image
size, since the interlacing can only split by row, not column.

Check out [pgm-interlace][pgm-interlace] for a tool to do this job.

## Notes

1. By default, this has your CPU over a barrel.
   It spawns as many threads as you have cores, so have fun with that.
   You can specify the number of threads you want it to (see syntax).

2. PGMs are grayscale, so you might want to tint it or apply a pallete of sort.
   I'm working on an example tool to do this, but there are so many palettes
   you could use that you might as well write your own.

[pgm-interlace]: https://github.com/phillid/pgm-interlace/
