#include "fractal-gen.h"

int main(int argc, char **argv)
{
	unsigned int size, iterat, cores, i, x, y, s;
	double power;
	void *(*generator)(void *);

	// Select correct generator for the fractal type
	// TO DO: use basename
	if (strcmp(argv[0], "./mbrot-gen") == 0)
		generator = &generate_mbrot_section;
	else if (strcmp(argv[0], "./bship-gen") == 0)
		generator = &generate_bship_section;
	else
		fprintf(stderr, "Don't call this directly, call a symlink to me\n");

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
		pthread_create(&sections[i].thread, NULL, generator, &(sections[i]));
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
