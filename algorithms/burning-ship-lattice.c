#include "../fractal-gen.h"

void *generate_burning_ship_lattice_section(void *section)
{
	data_section *d = (data_section*)section;
	unsigned int x,y,i;
	int idx = 0;
	double a,b;
	double complex z,c;
	double size_units = 0.09f;
	double top = -0.082f;
	double left = -1.8f;

	for (y = d->core, b = (d->core*(size_units/d->size)+top); y < d->size; b+=((d->cores*size_units)/d->size), y+=d->cores)
	{
		for (x = 0, a = left; x < d->size; a+=(size_units/d->size), x++)
		{
			z = 0;
			c = a+I*b;
			for (i = 0; i < d->iterat; i++)
			{
				if (cabsf(z) >= 2)
					break;

				z = cpow( cabsf(crealf(z)) + I*cabsf(cimagf(z)) , d->power) + c;
			}
			d->data[idx++] = (255*i)/d->iterat;
		}
	}
	return NULL;
}
