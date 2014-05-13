#include <stdio.h>
#include <complex.h>
#include <math.h>
#include "config.h"

int main(int argc, char **argv)
{
	unsigned int x, y, i;
	float a,b;
	float complex z,c;

	fprintf(stderr, "Creating %dx%d image (raw format)\n",SIZE,SIZE);

	y = 0;
	for (b = -1.75f; y < SIZE; b+=(3.5f/SIZE))
	{
		x = 0;
		for (a = -2.5f; x < SIZE; a+=(3.5f/SIZE))
		{
			z = 0;
			c = a+I*b;
			for (i = 0; i < ITERATIONS; i++)
			{
				if (cabsf(z) >= 2)
					break;

				z = cpow(z,POWER)+c;
			}

			printf("%c",( (255*i)/ITERATIONS ) );
			x++;
		}
		y++;
		if ( (y%10) == 0 )
			fprintf(stderr,"\r%.3f%%",y/(float)SIZE*100);
	}
	fprintf(stderr,"\n");
	return 0;
}
