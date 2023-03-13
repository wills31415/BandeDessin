#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <assert.h>

#include "Comic.h"
#include "SeamCarving.h"
#include "PNM.h"


double u(double x) {return 4 * x * (1 - x);}

int main(int argc, char* argv[])
{
    printf("size_t = %zu\n", (size_t)(-1));
    printf("unsigned int = %u\n", (unsigned int)(-1));
    printf("unsigned long = %lu\n", (unsigned long)(-1));
    printf("unsigned long long = %llu\n", (unsigned long long)(-1));

    return EXIT_SUCCESS;
}
