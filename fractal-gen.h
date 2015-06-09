#ifndef FRACTAL_GEN_H
#define FRACTAL_GEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
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

#include "algorithms.h"

#endif
