/* ------------------------------------------------------------------------- *
 * Seam carving.
 * Interface for Seam Carving.
 * ------------------------------------------------------------------------- */

#ifndef _SEAM_CARVING_H_
#define _SEAM_CARVING_H_

#include <stddef.h>
#include "PNM.h"

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


#endif // _SEAM_CARVING_H_
