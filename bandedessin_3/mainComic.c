#include <stdio.h>
#include <stdlib.h>

#include "Comic.h"
#include "PNM.h"


/* ------------------------------------------------------------------------- *\
 * NOM
 *      build_comic
 * SYNOPSIS
 *      build_comic width border comic.pnm [img1.pnm [img2.pnm ...]]
 * DESCIRPTION
 *      Build the comic corresponding to the given arguments
 * ARGUMENTS
 *      width      Page width
 *      border     Border thickness
 *      interval   Space between images
 *      comic      Output file name which will hold the comic (in PNM format)
 *      img        A ordered list of images (in PNM format) representing the
 *                 comic boxes
 \* ------------------------------------------------------------------------- */

int main(int argc, char* argv[])
{
    /* --- Argument parsing --- */
    if (argc < 6)
    {
        fprintf(stderr, "Usage: %s width border interval comic.pnm image1.pnm "
                        "image2.pnm ... imageN.pnm\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    size_t comicWidth = 0;
    if(sscanf (argv[1], "%zu", &comicWidth) != 1)
    {
        fprintf(stderr, "Aborting; Width should be an "
                        "unsigned integer. Got '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    size_t comicBorder = 0;
    if(sscanf (argv[2], "%zu", &comicBorder) != 1)
    {
        fprintf(stderr, "Aborting; Border should be an "
                        "unsigned integer. Got '%s'\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    size_t comicInterval = 0;
    if(sscanf (argv[3], "%zu", &comicInterval) != 1)
    {
        fprintf(stderr, "Aborting; Interval should be an "
                        "unsigned integer. Got '%s'\n", argv[3]);
        exit(EXIT_FAILURE);
    }


    size_t nbImages = argc - 5;

    PNMImage** images = (PNMImage**) malloc((nbImages) * sizeof(PNMImage*));
    if (!images)
    {
        fprintf(stderr, "Aborting; unable to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    // Load images
    for (size_t i = 0; i < nbImages; i++)
    {
        images[i] = readPNM(argv[i + 5]);

        if (!images[i])
        {
            fprintf(stderr, "Aborting; cannot load image '%s'\n", argv[i+5]);
            for (size_t k = 0; k < i; k++)
            {
                freePNM(images[k]);
            }
            free(images);
            exit(EXIT_FAILURE);
        }
    }

    /* --- Comic building --- */
    PNMImage* comic = packComic((const PNMImage**)images, nbImages, comicWidth,
                                comicBorder, comicInterval);
    if (!comic)
    {
        fprintf(stderr, "Aborting; cannot pack comic\n");
        for (size_t k = 0; k < nbImages; k++)
        {
            freePNM(images[k]);
        }

        free(images);
        exit(EXIT_FAILURE);
    }

    writePNM(argv[4], comic);

    // Free
    freePNM(comic);

    for (size_t k = 0; k < nbImages; k++)
    {
        freePNM(images[k]);
    }

    free(images);

    exit(EXIT_SUCCESS);
}
