/*
 * Part of fractal-gen - Generate iteration-based fractals in PNM format
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

#include "common.h"

void
*generate_mandelbrot_section(void *section)
{
	data_section *d = (data_section*)section;
	struct frame *f = &(d->parent_frame);
	unsigned int x,y,i;
	double a,b;
	double left, top;
	double complex z,c;

	defaultsd(&d->parent_frame.scale, 3.5f);
	defaultsd(&d->parent_frame.x, -0.75f);
	defaultsd(&d->parent_frame.y, 0);

	left = d->parent_frame.x - (d->parent_frame.scale / 2);
	top = d->parent_frame.y - (d->parent_frame.scale / 2);

	/* FIXME document this */
	b = clust_id*(f->scale/size)+top; /* FIXME document this */

	for (y = clust_id; y < size; y += clust_total) {
		a = d->core*(f->scale/size)+left;
		for (x = d->core; x < size; x += cores) {
			z = 0;
			c = a + I*b;
			for (i = 0; i < iterat; i++) {
				if (cabs(z) >= 2)
					break;

				z = cpow(z , power) + c;
			}
			d->data[d->idx++] = (255*i)/iterat;
			a += cores*(f->scale/size);
		}
		b += clust_total*(f->scale/size);
	}
	return NULL;
}
