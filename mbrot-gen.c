#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

typedef struct
{
	unsigned int core;
	unsigned int cores;
	unsigned int size;
	double power;
	unsigned int iterat;
	char* data;
	pthread_t thread;
} data_section;


void *generate_section(void *d);

int main(int argc, char **argv)
{
	unsigned int size, iterat, cores, i, x, y, s;
	double power;

	if (argc != 4)
	{
		fprintf(stderr, "%s size iterat power\n", argv[0]);
		return EXIT_FAILURE;
	}

	size   = atoi(argv[1]);
	iterat = atoi(argv[2]);
	power  = atof(argv[3]);


	// Fetch number of cores available on machine
	cores = sysconf(_SC_NPROCESSORS_ONLN);

	assert(size > 0);
	assert(iterat > 0);
	assert(cores > 0);



	// Allocated memory for sections, bailing upon failure
	data_section* sections = malloc(sizeof(data_section)*cores);
	if (sections == NULL)
	{
		perror("malloc");
		return EXIT_FAILURE;
	}

	fprintf(stderr, "Spawning %d threads:\n", cores);
	// Spawn all the threads! Something something interlacing
	for (i = 0; i < cores; i++)
	{
		// Has to be a better way
		sections[i].core = i;
		sections[i].cores = cores;
		sections[i].size = size;
		sections[i].power = power;
		sections[i].iterat = iterat;

		fprintf(stderr, " -> Thread #%d\n", i);
		pthread_create(&sections[i].thread, NULL, &generate_section, &(sections[i]));
	}

	// Wait for each thread to complete
	for (i = 0; i < cores; i++)
		pthread_join(sections[i].thread, NULL);

	// Vomit the data segments back onto the screen, deinterlacing
	// TO DO: look at fwrite performance benefits over putchar
	for (y = 0; y < size; y++)
		for (x = 0; x < size; x++)
			putchar(sections[y%cores].data[(y/cores)*size + x]);

	// Free the memory we allocated for point data
	for (s = 0; s < cores; s++)
		free(sections[s].data);

	free(sections);
	fprintf(stderr,"\n");
	return 0;
}


void *generate_section(void *section)
{
	data_section *d = (data_section*)section;
	unsigned int x,y,i;
	int idx = 0;
	double a,b;
	double complex z,c;


	d->data = malloc((d->size*d->size)/d->cores);
	if (d->data == NULL)
	{
		perror("malloc");
		return NULL;
	}


	for (y = d->core, b = (d->core*(3.5f/d->size)-1.75f); y < d->size; b+=((d->cores*3.5f)/d->size), y+=d->cores)
	{
		for (x = 0, a = -2.5f; x < d->size; a+=(3.5f/d->size), x++)
		{
			z = 0;
			c = a+I*b;
			for (i = 0; i < d->iterat; i++)
			{
				if (cabsf(z) >= 2)
					break;

				z = cpow(z, d->power)+c;
			}

			d->data[idx++] = (255*i)/d->iterat;
		}
	}
	return NULL;
}
