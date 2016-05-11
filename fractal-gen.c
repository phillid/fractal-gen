/*
 * fractal-gen - Generate iteration-based fractals in PNM format
 * Copyright (c) 2016 David Phillips <dbphillipsnz@gmail.com>
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "fractal-gen.h"
#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <sys/mman.h>
#include <string.h>

static struct section_generator generators[] = {
	{ "mandelbrot-gen" , &generate_mandelbrot_section },
	{ "burning-ship-gen" , &generate_burning_ship_section },
	{ "burning-ship-lattice-gen" , &generate_burning_ship_lattice_section }
};


int main(int argc, char **argv)
{
	unsigned long x = 0;
	unsigned long width = 0;
	size_t toalloc = 0;
	unsigned long y = 0;
	unsigned long i = 0;
	double ram_nice = 0.f; /* Forecast RAM usage, divided down to < 1024 */
	char* ram_unit = NULL; /* Unit for ram_nice */
	char* bname = NULL;
	data_section* sections = NULL;
	data_section *s = NULL;
	generator_func generator = NULL;
	pid_t child = 0;

	/* who are we? */
	argv0 = argv[0];

	/* Select correct generator for the fractal type */
	bname = basename(argv[0]);
	generator = select_generator(bname);

	if (generator == NULL)
	{
		fprintf(stderr, "Unknown fractal '%s' (perhaps try running symlink to me)\n",
			bname);
		return 1;
	}

	if (parse_args(argc, argv))
	{
		show_help();
		return 1;
	}

	/* Allocate memory for sections */
/*	if ((sections = malloc(sizeof(data_section)*cores)) == NULL)*/
	sections = mmap(NULL, sizeof(data_section)*cores, PROT_READ|PROT_WRITE,
	MAP_SHARED|MAP_ANONYMOUS, -1, 0);
/*	{
		perror("mmap");
		return 1;
	}*/

	ram_nice = (size*size)/clust_total;
	if (ram_nice < 1024)
		ram_unit = "B";
	else if (ram_nice < 1024*1024)
		ram_nice /= 1024, ram_unit = "KiB";
	else if (ram_nice < 1024*1024*1024)
		ram_nice /= (1024*1024), ram_unit = "MiB";
	else
		ram_nice /= (1024*1024*1024), ram_unit = "GiB";

	fprintf(stderr,
		"Forecast resource use:\n"
		" Threads: %d\n"
		" RAM    : ~%.4f %s\n",
		cores,
		ram_nice,
		ram_unit);
	/* Spawn all the threads! Something something interlacing */
	for (i = 0; i < cores; i++)
	{
		/* A bit complex, icky, will document later */
		if (i < (size%cores))
			width = (size/cores)+1;
		else
			width = (size/cores);

		toalloc = width*size;
		toalloc = ceilf((double)toalloc/clust_total);

		if ((sections[i].data = malloc(toalloc)) == NULL)
		{
			fprintf(stderr, "\nmalloc of %lu bytes failed\n", toalloc);
			perror("malloc");

			/* Free already allocated chunks of memory */
			i--;
			while(i-- + 1)
				free(sections[i].data);

			munmap(sections, sizeof(data_section)*cores);
			return 1;
		}
		sections[i].core = i;
		sections[i].width = width;
		sections[i].datasize = toalloc;
		fprintf(stderr, " -> Thread %lu\r", i);
		pthread_create(&sections[i].thread, NULL, generator, &(sections[i]));
	}

	s = &(sections[cores-1]);

	switch (child = fork())
	{
		case 0:
			while((x = s->idx) < s->datasize)
			{
				fprintf(stderr, "Thread %d: %.4f%%\r",
						cores-1,
						100.f*(double)x/s->datasize);
				sleep(1);
			}
			break;
		case -1:
			perror("progress thread fork");
		default:
			break;
	}
	/* Wait for each thread to complete */
	for (i = 0; i < cores; i++)
		pthread_join(sections[i].thread, NULL);

	kill(child, SIGKILL);

	/* Output PGM Header */
	printf("P5\n%d\n%d\n255\n",size,size/clust_total);

	/* Vomit the data segments onto stdout, interlacing frames from threads
	 * FIXME: look at buffering if at all possible */
	for (y = 0; y < size/clust_total; y++)
	{
		for (x = 0; x < size; x++)
		{
			s = &(sections[x%cores]);
			putchar(s->data[y*(s->width) + x/cores]);
		}
	}
	fprintf(stderr, "\nDone\n");

	/* Free the memory we allocated for point data */
	for (i = 0; i < cores; i++)
		free(sections[i].data);

	munmap(sections, sizeof(data_section)*cores);
	return 0;
}


int parse_args(int argc, char **argv)
{
	char opt = '\0';

	/* first things first: preload default or initial values */
	size = 0;
	iterat = 0;
	power = 2;
	cores = sysconf(_SC_NPROCESSORS_ONLN);
	thread_mult = 1;
	clust_id = 0;
	clust_total = 1;

	/* bail out early if no arguments are supplied */
	if (argc <= 1)
		return 1;

	while ( (opt = getopt(argc, argv, "s:i:e:c:t:N:T:")) != -1 )
	{
		switch (opt)
		{
			case 's': size = atoi(optarg); break;
			case 'i': iterat = atoi(optarg); break;
			case 'e': power = atof(optarg); break;
			case 'c': cores = atoi(optarg); break;
			case 't': thread_mult = atof(optarg); break;
			case 'N': clust_id = atoi(optarg); break;
			case 'T': clust_total = atoi(optarg); break;
			/* redundant case for '?', but explicitness is best */
			case '?':
			default:
				return 1;
				break;
		}
	}

	/* Extend number of threads to multiplier value */
	cores *= thread_mult;

	if (size <= 0)
	{
		fprintf(stderr, "ERROR: size must be positive\n");
		return 1;
	}

	if (iterat <= 0)
	{
		fprintf(stderr, "ERROR: max iteration count must be positive\n");
		return 1;
	}

	if (clust_id >= clust_total)
	{
		fprintf(stderr, "WARN: Node ID cannot be >= node count\n");
		return 1;
	}

	/* Interlacing is row-based, can't have more workers than columns */
	if (cores > size)
	{
		cores = size;
		fprintf(stderr, "WARN: Capping number of threads to image size (%d)\n", cores);
	}

	if (size % clust_total != 0)
	{
		fprintf(stderr, "ERROR: image size must be an exact multiple of clust_total\n");
		return 1;
	}

	if (cores <= 0)
	{
		fprintf(stderr, "ERROR: core counts should be positive\n");
		return 1;
	}
	return 0;
}


generator_func select_generator(const char* name)
{
	unsigned long i = 0;
	for (i = 0; i < sizeof(generators)/sizeof(struct section_generator); i++)
		if (strcmp(name, generators[i].executable_name) == 0)
			return generators[i].generator;

	return NULL;
}

void show_help()
{
	fprintf(stderr,
			"%s -s size -i iterat [-e exponent]\n"
			"        [-c cores] [-t thread_multiplier]\n"
			"        [-N cluster-id -T cluster-total]\n",
			argv0);
}
