#include "fractal-gen.h"

struct section_generator
{
	char *executable_name;
	void *(*generator)(void *);
};

int main(int argc, char **argv)
{
	unsigned int size, iterat, cores, i, x, y;
	double power;
	char* bname;
	data_section* sections;
	void *(*generator)(void *);

	struct section_generator generators[] = {
		{ "mandelbrot-gen" , &generate_mandelbrot_section },
		{ "burning-ship-gen" , &generate_burning_ship_section },
		{ "tricorn-gen" , &generate_tricorn_section }
		};

	// Select correct generator for the fractal type
	bname = basename(argv[0]);
	generator = NULL;
	for (i = 0; i < sizeof(generators)/sizeof(struct section_generator); i++)
		if (strcmp(bname, generators[i].executable_name) == 0)
			generator = generators[i].generator;

	if (generator == NULL)
	{
		fprintf(stderr, "Don't call this directly, call a symlink to me\n");
		return EXIT_FAILURE;
	}

	if (argc != 4 && argc != 5)
	{
		fprintf(stderr, "%s size iterat power [threads]\n", argv[0]);
		return EXIT_FAILURE;
	}

	size   = atoi(argv[1]);
	iterat = atoi(argv[2]);
	power  = atof(argv[3]);

	// Fetch number of cores available on machine
	cores = argc == 5? atoi(argv[4]) : sysconf(_SC_NPROCESSORS_ONLN); 	// Screw maintainability ;)

	// Interlacing is column-based, can't have more workers than columns
	if (cores > size)
	{
		fprintf(stderr, "WARN: Capping number of threads to image width\n");
		cores = size;
	}

	assert(size > 0);
	assert(iterat > 0);
	assert(cores > 0);

	// Allocate memory for sections
	if ((sections = malloc(sizeof(data_section)*cores)) == NULL)
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

		// A bit complex, icky, will document later
		if (i < (size%cores))
			x = (size*((int)(size/cores)+1));
		else
			x = (size*(int)(size/cores));

		if ((sections[i].data = malloc(x)) == NULL)
		{
			fprintf(stderr, "\n");
			perror("malloc");
			// Free already allocated chunks of memory
			i--;
			while(i-- + 1)
				free(sections[i].data);

			free(sections);
			return EXIT_FAILURE;
		}
		fprintf(stderr, " -> Thread #%d (%d bytes data area)\r", i+1, x);
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

	fprintf(stderr, "\nDone\n");

	// Free the memory we allocated for point data
	for (i = 0; i < cores; i++)
		free(sections[i].data);

	free(sections);
	return 0;
}
