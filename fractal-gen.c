#include "fractal-gen.h"

int main(int argc, char **argv)
{
	unsigned int size, iterat, cores, i, x, y;
	double power;
	char* bname;
	void *(*generator)(void *);

	// Select correct generator for the fractal type
	bname = basename(argv[0]);
	if (strcmp(bname, "mbrot-gen") == 0)
	{
		generator = &generate_mbrot_section;
	} else if (strcmp(bname, "bship-gen") == 0) {
		generator = &generate_bship_section;
	} else {
		fprintf(stderr, "Don't call this directly, call a symlink to me\n");
		return EXIT_FAILURE;
	}

	if (argc != 4)
	{
		fprintf(stderr, "%s size iterat power\n", argv[0]);
		return EXIT_FAILURE;
	}

	size   = atoi(argv[1]);
	iterat = atoi(argv[2]);
	power  = atof(argv[3]);


	// Fetch number of cores available on machine
	// FIXME make the num threads selectable
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

	// Spawn all the threads! Something something interlacing
	fprintf(stderr, "Spawning %d threads:\n", cores);
	for (i = 0; i < cores; i++)
	{
		// Has to be a better way
		sections[i].core = i;
		sections[i].cores = cores;
		sections[i].size = size;
		sections[i].power = power;
		sections[i].iterat = iterat;
		sections[i].data = malloc((size*size)/cores);
		if (sections[i].data == NULL)
		{
			// Free already allocated chunks of memory
			while(i-- + 1)
				free(sections[i].data);

			free(sections);
			perror("malloc");
			return EXIT_FAILURE;
		}
		fprintf(stderr, " -> Thread #%d\n", i);
		pthread_create(&sections[i].thread, NULL, generator, &(sections[i]));
	}

	// Wait for each thread to complete
	for (i = 0; i < cores; i++)
		pthread_join(sections[i].thread, NULL);

	// Output PGM Header
	printf("P5\n%d\n%d\n255\n",size,size);

	// Vomit the data segments back onto the screen, deinterlacing
	// TO DO: look at fwrite performance benefits over putchar
	for (y = 0; y < size; y++)
		for (x = 0; x < size; x++)
			putchar(sections[y%cores].data[(y/cores)*size + x]);

	// Free the memory we allocated for point data
	for (i = 0; i < cores; i++)
		free(sections[i].data);

	free(sections);
	fprintf(stderr,"\n");
	return 0;
}
