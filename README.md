# Fractal Gen
This is a tiny program which will output a binary [PGM](https://wikipedia.org/wiki/Netpbm_format#PGM_example) format square image of a mandelbrot or burning ship set.


## Syntax

	./mandelbrot-gen -s size -i max_iterations [-e exponent] \
	                 [-c cores] [-t thread_multiplier]
	                 [-N cluster-id -T cluster-total] > output.pgm

Also check out the other algorithms like `tricorn-gen` and `burning-ship-gen`.
You might then want to consider using a tool such as ImageMagick to then convert the (large) output file into something more sane like a PNG.
For the clich&eacute; set you'll want to keep the exponent at 2.
For more info on the exponent, read through [Wikipedia's fine article](http://wikipedia.org/wiki/Mandelbrot_set).

### Options in detail

The `-s` and `-i` are the only two mandatory arguments. All others will assume
their default values (shown here in brackets) if left unspecified.

Also note that `-N` should always be used together with `-T`.

	-s size - Integer. Height and width of image in pixels
	-s max_iterations - Integer. Maximum number of iterations to use to
	                    determine if number is in the fractal set.
	-e exponent - Float. Exponent to use in the fractal's calculation. (2)
	-c cores - Integer > 0. Number of cores to assume computer has.
			   (sysconf(_SC_NPROCESSORS_ONLN))
	-t thread_multiplier - Integer > 0. Coefficiant to multiply cores by in
	                       order to achieve count of worker threads to use. (1)
	-N cluster_id - 0 <= Integer < cluster_total. A unique ID used to determine
	                which sections of the image this instance should work on.
	                Clustering explained in the 'Clustering' section of this
	                document. (0)
	-T cluster_total - 0 < Integer <= size. Total count of worker instances in
	                   this cluster.

## Clustering

This program allows workload for an image to be spread across multiple
instances of the program. This means, for example, that you can parallel the
computation on an image between multiple computers.

### Example

Let's say we're generating a quick mandelbrot 20000 pixels in width and with
100 iterations max. We want to spread the load between two identical computers
in order to perhaps halve the computation time:

	# On computer #1:
	./mandelbrot -s 20000 -i 100 -T 2 -N 0 > 0.pgm
	# On computer #2:
	./mandelbrot -s 20000 -i 100 -T 2 -N 1 > 1.pgm

At the heart of it, each instance will only calculate half of the image. This
is achieved by (in this case) getting the instance with ID 0 to work on
the 1st, 3rd, 5th, and other odd numbered columns, while the instance with ID 1
will work on the columns in between. To perhaps point out the obvious(?),
interlacing is used in a bid to spread the workload more evenly across cluster
members.

This will result in two separate pgm files, each 10000x20000 pixels.
They must be vertically interlaced in order to form the whole desired image.
This extends to any number of cluster members less than or equal to the image
size, since the interlacing can only split by column, not row.

I currently have a tool that I use to interlace the resultant images. I'm
working on polishing it so it is publish-worthy.

## Notes

1. By default, this has your CPU over a barrel.
   It spawns as many threads as you have cores, so have fun with that.
   You can specify the number of threads you want it to (see syntax).

2. PGMs are grayscale, so you might want to tint it or apply a pallete of sort.
