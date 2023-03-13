/* ------------------------------------------------------------------------- *
 * Seam carving.
 * Interface for Seam Carving.
 * ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <pthread.h>
#include <assert.h>

#include "SeamCarving.h"



/* Calcul de l'énergie d'un pixel. */
static int E(size_t i, size_t j, const PNMImage* image)     // À TESTER
{
    /* Vérification de la validité des paramètres de la fonction. */
    assert(i >= 0);
    assert(i < image->height);

    assert(j >= 0);
    assert(j < image->width);

    assert(image != NULL);


    /* Traitements des cas particuliers. */
    size_t i1 = (i == 0 ? i : i - 1);
    size_t i2 = (i == image->height - 1 ? i : i + 1);

    size_t j1 = (j == 0 ? j : j - 1);
    size_t j2 = (j == image->width - 1 ? j : j + 1);

    /* Calcul de l'énergie du pixel. */
    int e = 0;

    e += abs(image->data[i1 * image->width + j].red - image->data[i2 * image->width + j].red) / 2;
    e += abs(image->data[i * image->width + j1].red - image->data[i * image->width + j2].red) / 2;

    e += abs(image->data[i1 * image->width + j].green - image->data[i2 * image->width + j].green) / 2;
    e += abs(image->data[i * image->width + j1].green - image->data[i * image->width + j2].green) / 2;

    e += abs(image->data[i1 * image->width + j].blue - image->data[i2 * image->width + j].blue) / 2;
    e += abs(image->data[i * image->width + j1].blue - image->data[i * image->width + j2].blue) / 2;

    return e;
}


PNMImage* showEnergies(const PNMImage* image)       // FONCTIONNE
{
    /* Vérification de la validité des paramètres de la fonction. */
    if(image == NULL) return NULL;

    /* Création de la carte des énergies. */
    PNMImage* heatmap = createPNM(image->width, image->height);
    if(heatmap == NULL) return NULL;

    /* Calcul de la carte des énergies. */
    unsigned char e;

    for(size_t i = 0; i < image->height; ++i)
    {
        for(size_t j = 0; j < image->width; ++j)
        {
            e = E(i, j, image);
            heatmap->data[i * heatmap->width + j].red = e;
            heatmap->data[i * heatmap->width + j].green = e;
            heatmap->data[i * heatmap->width + j].blue = e;
        }
    }

    return heatmap;
}


static void get_seam_rec(size_t i, size_t j, int* m, int* s, PNMImage* cost, size_t* p, size_t k)
{
    size_t W = cost->width;

    /* (s[i * W + p[i * W + j]] != -1) ==> "m[i * W + p[i * W + j]]" déjà calculé : rien à faire. */
    if(s[i * W + p[i * W + j]] != -1) return;

    /* (i == 0) ==> On est sur la 1ère ligne de l'image. */
    if(i == 0)
    {
        /* On est sur la 1ère ligne, pas de valeur à calculer pour "s[i * W + p[i * W + j]]", donc -2 par défaut. */
        s[i * W + p[i * W + j]] = -2;
        /* On est sur la 1ère ligne, "m[i * W + p[i * W + j]]" prend la valeur de l'énergie du pixel sur la colonne "p[i * W + j]". */
        m[i * W + p[i * W + j]] = (int)cost->data[i * W + p[i * W + j]].red;
    }
    /* (i != 0) ==> On n'est pas sur la 1ère ligne de l'image : On doit calculer "s[i * W + p[i * W + j]]" et "m[i * W + p[i * W + j]]". */
    else
    {
        /* Bornes pour la recherche du min : on gère ici les cas où on est au bord de l'image. */
        size_t j1 = (j == 0 ? j : j - 1);
        size_t j2 = (j == W - 1 - k ? j : j + 1);

        /* Initialisation des variables pour la recherche du min. */
        s[i * W + p[i * W + j]] = p[i * W + j];
        m[i * W + p[i * W + j]] = -1;

        /* Recherche du min parmi les pixels de la ligne au-dessus situés entre les colonnes "p[i * W + j1]" et "p[i * W + j2]". */
        for(size_t l = j1; l <= j2; ++l)
        {
            //printf("i = %zu\tj = %zu\tj1 = %zu\tj2 = %zu\tl = %zu\tp[l] = %zu\n", i, j, j1, j2, l, p[i * W + l]);
            /* Appel récursif de la fonction pour calculer "m" et "s" si besoin. */
            get_seam_rec(i - 1, l, m, s, cost, p, k);

            /* Si un nouveau min est trouvé, on affecte les nouvelles valeurs à "m" et "s". */
            if(m[i * W + p[i * W + j]] == -1 || m[i * W + p[i * W + j]] >= m[(i-1) * W + p[(i - 1) * W + l]] + (int)cost->data[i * W + p[i * W + j]].red)
            {
                /* "s[i * W + p[i * W + j]]" prend l'indice de la colonne du min. */
                s[i * W + p[i * W + j]] = p[(i - 1) * W + l];
                /* "m[i * W + p[i * W + j]]" prend la somme des énergies du min et du pixel (i, p[i * W + j]). */
                m[i * W + p[i * W + j]] = m[(i-1) * W + p[(i - 1) * W + l]] + (int)cost->data[i * W + p[i * W + j]].red;
            }
        }
    }
}


size_t* findSeams(const PNMImage* image, size_t k)
{
    /* Vérification de la validité des paramètres de la fonction. */
    assert(image != NULL);
    if(k == 0) return NULL;
    assert(k <= image->width);

    /* Initialisation du tableau "path". */
    size_t* path = (size_t*)calloc(k * image->height, sizeof(size_t));

    /* Calcul de "heatmap", le gradient de "image". */
    PNMImage* heatmap = showEnergies(image);

    /* Initialisation des tableaux pour la recherche dynamique de la couture optimale. */
    int* m = (int*)calloc(image->height * image->width, sizeof(int));
    int* s = (int*)calloc(image->height * image->width, sizeof(int));
    for(size_t k = 0; k < image->height * image->width; ++k) s[k] = -1;

    /* Initialisation du tableau "p" correspondant aux indices horizontaux des pixels, décalés en fonction des coutures précédentes. */
    size_t* p = (size_t*)calloc(image->height * image->width, sizeof(size_t));
    for(size_t i = 0; i < image->height; ++i)
    for(size_t j = 0; j < image->width; ++j)
    {
        p[i * image->width + j] = j;
        //printf("p(%zu, %zu) = %zu\n", i, j, p[i * image->width + j]);
    }

    /* Calcul des valeurs des tableaux "m" et "s" pour la recherche de la 1ère couture. */
    for(size_t j = 0; j < image->width; ++j)
        get_seam_rec(image->height - 1, j, m, s, heatmap, p, 0);

    /* Déclaration d'une variable pour la recherche du min. */
    int min = -1;

    /* Recherche de la 1ère couture. */
    for(size_t j = 0; j < image->width; ++j)
    {
        if(min == -1 || min > m[(image->height - 1) * image->width + j])
        {
            min = m[(image->height - 1) * image->width + j];
            path[image->height - 1] = j;
        }
    }

    /* Calcul des valeurs du tableau "path" pour la 1ère couture. */
    for(size_t i = image->height - 2; i >= 0; --i)
    {
        path[i] = s[(i + 1) * image->width + path[i + 1]];
        if(!i) break;
    }

    /* Déclaration de variables pour le calcul de "heatmap" pour chaque couture. */
    size_t i1, i2, j1, j2;

    /* Calcul des coutures suivantes. */
    for(size_t i = 1; i < k; ++i)
    {
        /* Réinitialisation des valeurs du tableau "s" à -1. */
        for(size_t j = 0; j < image->height * image->width; ++j) s[j] = -1;

        /* Modification des valeurs de "p" en fonction de la dernière couture. */
        for(size_t j = 0; j < image->height; ++j)
        for(size_t l = 0; l < image->width - i; ++l)
        {
            /* Si le pixel (j, l) est à droite de la dernière couture : on décale toute la ligne à partir de "l". */
            if(p[j * image->width + l] >= path[(i - 1) * image->height + j])
                p[j * image->width + l] = p[j * image->width + (l + 1)];
                //printf("p(%zu, %zu) = %zu\n", j, l, p[j * image->width + l]);
        }

        /* Calcul des valeurs de "heatmap" en fonction de la couture précédente.*/
        for(size_t I = 0; I < image->height; ++I)
        for(size_t J = 0; J < image->width - i; ++J)
        {
            i1 = (I == 0 ? I : I - 1);
            i2 = (I == image->height - 1 ? I : I + 1);

            j1 = (J == 0 ? J : J - 1);
            j2 = (J == image->width - 1 - i ? J : J + 1);

            min = 0;

            min += abs(image->data[i1 * image->width + p[i1 * image->width + J]].red - image->data[i2 * image->width + p[i2 * image->width + J]].red) / 2;
            min += abs(image->data[I * image->width + p[I * image->width + j1]].red - image->data[I * image->width + p[I * image->width + j2]].red) / 2;

            min += abs(image->data[i1 * image->width + p[i1 * image->width + J]].green - image->data[i2 * image->width + p[i2 * image->width + J]].green) / 2;
            min += abs(image->data[I * image->width + p[I * image->width + j1]].green - image->data[I * image->width + p[I * image->width + j2]].green) / 2;

            min += abs(image->data[i1 * image->width + p[i1 * image->width + J]].blue - image->data[i2 * image->width + p[i2 * image->width + J]].blue) / 2;
            min += abs(image->data[I * image->width + p[I * image->width + j1]].blue - image->data[I * image->width + p[I * image->width + j2]].blue) / 2;

            heatmap->data[I * image->width + p[I * image->width + J]].red = min;
        }

        /* Calcul des valeurs des tableaux "m" et "s" pour la couture "i". */
        for(size_t j = 0; j < image->width - i; ++j)
            get_seam_rec(image->height - 1, j, m, s, heatmap, p, i);

        /* Recherche de la nouvelle couture d'énergie minimale. */
        min = -1;
        for(size_t j = 0; j < image->width - i; ++j)
        {
            if(min == -1 || min > m[(image->height - 1) * image->width + p[(image->height - 1) * image->width + j]])
            {
                min = m[(image->height - 1) * image->width + p[(image->height - 1) * image->width + j]];
                path[i * image->height + (image->height - 1)] = p[(image->height - 1) * image->width + j];
            }
        }

        /* Calcul des valeurs du tableau "path" pour la nouvelle couture. */
        for(size_t j = image->height - 2; j >= 0; --j)
        {
            path[i * image->height + j] = s[(j + 1) * image->width + path[i * image->height + (j + 1)]];
            if(!j) break;
        }
    }

    /* Destruction de "heatmap" et des tableaux locaux à la fonction. */
    freePNM(heatmap);
    free(m);
    free(s);
    free(p);


    return path;
}


PNMImage* showSeams(const PNMImage* image, size_t k)
{
    /* Vérification de la validité des paramètres. */
    if(image == NULL) return NULL;
    if(k == 0) return NULL;
    if(k > image->width) return NULL;

    /* Calcul des coutures optimales. */
    size_t* path = findSeams(image, k);

    /* Création de "outputImage", une copie de "image". */
    PNMImage* outputImage = createPNM(image->width, image->height);

    for(size_t i = 0; i < image->height; ++i)
    for(size_t j = 0; j < image->width; ++j)
        outputImage->data[i * image->width + j] = image->data[i * image->width + j];

    /* Affichage des coutures sur "outputImage". */
    PNMPixel white = {255, 255, 255};

    for(size_t i = 0; i < k; ++i)
    for(size_t j = 0; j < image->height; ++j)
    {
        outputImage->data[j * image->width + path[i * image->height + j]] = white;
        //printf("path(%zu, %zu) = %zu\n", i, j, path[i * image->height + j]);
    }

    /* Destruction des tableaux locaux à la fonction. */
    free(path);


    return outputImage;
}


PNMImage* reduceImageWidth(const PNMImage* image, size_t k)
{
    /* Vérification de la validité des paramètres. */
    if(image == NULL) return NULL;
    if(k >= image->width) return NULL;

    /* Création de "outputImage", une copie réduite de "image". */
    PNMImage* outputImage = createPNM(image->width - k, image->height);

    /* Dans le cas où "k" est nul : on renvoie une copie de "image". */
    if(!k)
    {
        for(size_t i = 0; i < image->width * image->height; ++i) outputImage->data[i] = image->data[i];

        return outputImage;
    }

    /* Recherche des "k" coutures optimales. */
    size_t* path = findSeams(image, k);

    /* Création du tableau "p", de taille "image->width", contenant les indices horizontaux modifiés. */
    size_t* p = (size_t*)calloc(image->width + 1, sizeof(size_t));

    /* Copie ligne par ligne de "image" réduite dans "outputImage". */
    for(size_t i = 0; i < outputImage->height; ++i)
    {
        /* Initialisation du tableau "p" pour la ligne courante. */
        for(size_t j = 0; j <= image->width; ++j) p[j] = j;

        /* Modification des indices horizontaux dans "p". */
        for(size_t k0 = 0; k0 < k; k0++)
        {
            /* Pour chaque couture, décalage vers la gauche de toutes les valeurs de "p" à droite d'une couture. */
            for(size_t j = 0; j < image->width; ++j)
            {
                if(p[j] >= path[k0 * image->height + i]) p[j] = p[j + 1];
            }
        }

        /* Copie de la ligne courante réduite de "image" dans "outputImage". */
        for(size_t j = 0; j < outputImage->width; ++j)
        {
            outputImage->data[i * outputImage->width + j] = image->data[i * image->width + p[j]];
        }
    }

    /* Destruction des tableaux locaux à la fonction. */
    free(path);
    free(p);


    return outputImage;
}

PNMImage* increaseImageWidth(const PNMImage* image, size_t k)
{
    /* Vérification de la validité des paramètres. */
    if(image == NULL) return NULL;

    /* Création d'une variable pour le cas où l'image doit (au moins) doubler de largeur. */
    size_t k_iter = (k <= image->width ? 0 : k - image->width);

    /* Dans le cas ci-dessus, on modifie la valeur de "k" pour effectuer l'itération courante de la fonction. */
    if(k_iter > 0) k = image->width;

    /* Création de "outputImage", une copie agrandie de "image". */
    PNMImage* outputImage = createPNM(image->width + k, image->height);

    /* Dans le cas où "k" est nul : on renvoie une copie de "image". */
    if(!k)
    {
        for(size_t i = 0; i < image->width * image->height; ++i) outputImage->data[i] = image->data[i];

        return outputImage;
    }

    /* Recherche des "k" coutures optimales. */
    size_t* path = findSeams(image, k);

    /* Création du tableau "p", de taille "image->width + k", contenant les indices horizontaux modifiés. */
    size_t* p = (size_t*)calloc(image->width + k, sizeof(size_t));

    /* Déclaration d'une variable pour la recherche des valeurs de "p" à décaler, ligne par ligne. */
    size_t j0 = 0;

    /* Copie ligne par ligne de "image" agrandie dans "outputImage". */
    for(size_t i = 0; i < outputImage->height; ++i)
    {
        /* Initialisation du tableau "p" pour la ligne courante. */
        for(size_t j = 0; j < image->width + k; ++j) p[j] = j;

        /* Modification des indices horizontaux dans "p". */
        for(size_t k0 = 0; k0 < k; k0++)
        {
            /* Pour chaque couture, décalage vers la droite de toutes les valeurs de "p" à droite d'une couture. */
            /* Recherche de l'indice à partir duquel on décale vers la droite. */
            for(j0 = 0; j0 < image->width + k; ++j0)
            {
                if(p[j0] >= path[k0 * image->height + i]) break;
            }

            /* Décalage vers la droite de toutes les valeurs de "p" après l'indice "j0". */
            for(size_t j = image->width + k - 1; j > j0; --j) p[j] = p[j - 1];
        }

        /* Copie de la ligne courante augmentée de "image" dans "outputImage". */
        for(size_t j = 0; j < outputImage->width; ++j)
        {
            outputImage->data[i * outputImage->width + j] = image->data[i * image->width + p[j]];
        }
    }

    /* Destruction des tableaux locaux à la fonction. */
    free(path);
    free(p);

    /* Appel récursif à la fonction pour le cas où l'image doit (au moins) doubler de largeur. */
    if(k_iter > 0)
    {
        PNMImage* tmpImage = outputImage;
        outputImage = increaseImageWidth(tmpImage, k_iter);

        freePNM(tmpImage);
    }


    return outputImage;
}

void* thread_modifyImageWidth(void* arg)
{
    /* Cast de  "arg". */
    arg_t* input = (arg_t*)arg;

    /* Vérification da la validité des paramètres de la fonction. */
    if(!input) pthread_exit(NULL);
    if(!(input->image)) pthread_exit(NULL);

    /* Déclaration de la variable "output", copie redimensionnée de "input->image". */
    PNMImage* output = NULL;

    /* Dans le cas où "input->k" est nul : on renvoie une copie de "input->image". */
    if(!(input->k))
    {
        output = createPNM(input->image->width, input->image->height);
        for(size_t i = 0; i < output->width * output->height; ++i) output->data[i] = input->image->data[i];

        pthread_exit((void*)output);
    }

    /* Modification de la largeur de l'image selon la valeur de "input->k". */
    if(input->k < 0) output = reduceImageWidth(input->image, (size_t)(-input->k));
    else output = increaseImageWidth(input->image, (size_t)(input->k));


    pthread_exit((void*)output);
}
