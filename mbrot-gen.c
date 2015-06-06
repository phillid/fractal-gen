#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

typedef struct
{
	int core;
	int cores;
	int size;
	double power;
	unsigned int iterat;
	char* data;
	pthread_t thread;
} data_section;


void *generate_section(void *d);



int main(int argc, char **argv)
{
	int size, iterat, cores,i;
	double power;

	if (argc != 4)
	{
		fprintf(stderr, "%s size iterat power", argv[0]);
		return EXIT_FAILURE;
	}

	size   = atoi(argv[1]);
	iterat = atoi(argv[2]);
	power  = atof(argv[3]);


	// Fetch number of cores available on machine, so we can use lots of them to speed things up
	cores = sysconf (_SC_NPROCESSORS_ONLN);
//	cores = 8;

	// TO DO: assertions for size, iterat, power and cores


	fprintf(stderr, "Found %d cores. I'll spawn as many threads and let the scheduler sort it out.\n", cores);

	// TO DO: move to top of function
	int x,y,s;

	data_section* sections = malloc(sizeof(data_section)*cores);

	fprintf(stderr, "Spawning threads:\n");
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

	fprintf(stderr,"\n");
	return 0;
}


void *generate_section(void *section)
{
	data_section *d = (data_section*)section;
	d->data = malloc((d->size*d->size)/d->cores);
	unsigned int x,y,i;
	double a,b;
	double complex z,c;

	int idx = 0;

	// TO DO: assert malloc succeeded

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
