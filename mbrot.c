#include "fractal-gen.h"

void *generate_mbrot_section(void *section)
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
