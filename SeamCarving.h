/* ------------------------------------------------------------------------- *
 * Seam carving.
 * Interface for Seam Carving.
 * ------------------------------------------------------------------------- */

#ifndef _SEAM_CARVING_H_
#define _SEAM_CARVING_H_

#include <stddef.h>
#include "PNM.h"


// Types ----------------------------------------------------------------------

typedef struct
{
    size_t* nb_images_done;
    pthread_mutex_t* mutex;
} shared_arg_t;


typedef struct
{
    const PNMImage* image;
    long k;

    shared_arg_t shared;
} arg_t;


// Methods --------------------------------------------------------------------

/* ------------------------------------------------------------------------- *
 * Show the energy heatmap
 *
 * The PNM image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * image        Pointer to a PNM image
 *
 * RETURN
 * image        Pointer to a new PNM image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage* showEnergies(const PNMImage* image);

/* ------------------------------------------------------------------------- *
* Find the best k vertical seams of minimun energy
* using the seam carving algorithm.
*
* The returned array must later be deleted
* by calling free(path[0]), then free(path) in this order.
*
* PARAMETERS
* image        Pointer to a PNM image
* k            The number of seams to find (along the width axis).
*
* RETURN
* path         A 2 dimensional array of size k by image->height,
*              such that, for any given 0 <= i < k, and 0 <= j < image->height,
*              path[i * image->height + j] corresponds
*              to the horizontal position of the j-th pixel
*              along the i-th optimal vertical seam.
* NULL         if an error occured
* ------------------------------------------------------------------------- */
size_t* findSeams(const PNMImage* image, size_t k);

/* ------------------------------------------------------------------------- *
 * Show the minimal energy seams from the seam carving algorithm
 *
 * The PNM image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * image        Pointer to a PNM image
 * k            The number of seams to be drawn (along the width axis)
 *
 * RETURN
 * image        Pointer to a new PNM image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage* showSeams(const PNMImage* image, size_t k);

/* ------------------------------------------------------------------------- *
 * Reduce the width of a PNM image to `image->width-k` using the seam carving
 * algorithm.
 *
 * The PNM image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * image        Pointer to a PNM image
 * k            The number of pixels to be removed (along the width axis)
 *
 * RETURN
 * image        Pointer to a new PNM image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage* reduceImageWidth(const PNMImage* image, size_t k);

/* ------------------------------------------------------------------------- *
 * Increase the width of a PNM image to `image->width+k` using the seam
 * carving algorithm.
 *
 * The PNM image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * image        Pointer to a PNM image
 * k            The number of pixels to be added (along the width axis)
 *
 * RETURN
 * image        Pointer to a new PNM image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage* increaseImageWidth(const PNMImage* image, size_t k);

/* ------------------------------------------------------------------------- *
 * Function called in a thread to modify the width of a PNM image.
 *
 * Both "input" and "output" are considered as "void*" pointers by the function.
 *
 * PARAMETERS
 * input        A pointer to a structure containing the required parameters
 *              to resize the image.
 *
 * RETURN
 * output       A copy of the image resized.
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
void* thread_modifyImageWidth(void* input);


#endif // _SEAM_CARVING_H_
