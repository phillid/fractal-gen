/*
 * Part of fractal-gen - Generate iteration-based fractals in PNM format
 * Copyright (c) 2015 David Phillips <dbphillipsnz@gmail.com>
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

#include "common.h"

void *generate_mandelbrot_section(void *section)
{
	data_section *d = (data_section*)section;
	unsigned int x,y,i;
	double a,b;
	double complex z,c;
	double size_units = 3.5f;
	double top = -1.75f;
	double left = -2.5f;


	/* FIXME document this */
	b = (d->core*(size_units/size)+top);

	for (y = d->core; y < size; y += cores)
	{
		a = clust_id*(size_units/size)+left; /* FIXME document this */
		for (x = clust_id; x < size; x+=clust_total)
		{
			z = 0;
			c = a + I*b;
			for (i = 0; i < iterat; i++)
			{
				if (cabs(z) >= 2)
					break;

				z = cpow(z , power) + c;
			}
			d->data[d->idx++] = (255*i)/iterat;
			a += (clust_total*size_units)/size;
		}
		b += (cores*size_units)/size;
	}
	return NULL;
}
