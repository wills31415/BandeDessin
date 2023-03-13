/* ------------------------------------------------------------------------- *
 * Comic.
 * Interface for packing a comics.
 * ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <pthread.h>

#include "Comic.h"



/* Fonction récursive de calcul dynamique du placement de coût optimal. */
static void get_optimal_cost_rec(size_t i, size_t j, int* m, int* s, int* cost, size_t N)
{
    /* Si le coût optimal a déjà été calculé, on ne fait rien. */
    if(s[i * N + j] != -1) return;

    /* S'il n'y a qu'une seule image sur la ligne. */
    if(i == j)
    {
        s[i * N + j] = i;
        m[i * N+ j] = cost[i * N + j];
    }
    /* Si on veut mettre un nombre négatif d'images sur la ligne. */
    else if(i > j)
    {
        s[i * N + j] = N;
        m[i * N + j] = 0;
    }
    /* Sinon, on calcul le coût optimal en fonction de ceux déjà calculés.*/
    else
    {
        s[i * N + j] = j;
        m[i * N + j] = cost[i * N + j];

        for(size_t k = i; k < j; ++k)
        {
            get_optimal_cost_rec(i, k, m, s, cost, N);
            get_optimal_cost_rec(k+1, j, m, s, cost, N);

            if(m[i * N + j] >= m[i * N + k] + m[(k+1) * N + j])
            {
                s[i * N + j] = k;
                m[i * N + j] = m[i * N + k] + m[(k+1) * N + j];
            }
        }
    }
}


/* Fonction récursive de calcul de la position des cases sur l'image d'après le résultat de la recherche dynamique du coût optimal. */
static void get_positions_rec(size_t i, size_t j, size_t* positions, int* s, size_t N)
{
    size_t pivot = s[i * N + j];
    if(pivot == j) return;
    for(size_t k = pivot + 1; k < N; ++k) positions[k] += 1;
    get_positions_rec(i, pivot, positions, s, N);
    get_positions_rec(pivot + 1, j, positions, s, N);
}


size_t* wrapImages(const PNMImage** images, size_t nbImages, size_t comicWidth, size_t comicBorder, size_t comicInterval)         // FONCTIONNE
{
    /* Vérification de la validité des paramètres de la fonction. */
    if(images == NULL) return NULL;
    if(nbImages == 0) return NULL;

    /* Création et initialisation du tableau "cost" de dimensions (nbImages x nbImages). */
    int* cost = (int*)calloc(nbImages * nbImages, sizeof(int));

    for(size_t i = 0; i < nbImages; ++i)
    {
        for(size_t j = i; j < nbImages; ++j)
        {
            cost[i * nbImages + j] = comicWidth - 2 * comicBorder;
            for(size_t k = i; k <= j; ++k)
            {
                cost[i * nbImages + j] -= images[k]->width;
                if(k != j) cost[i * nbImages + j] -= comicInterval;
            }
            cost[i * nbImages + j] = abs(cost[i * nbImages + j]);
        }
    }

    int* m = (int*)calloc(nbImages * nbImages, sizeof(int));
    int* s = (int*)calloc(nbImages * nbImages, sizeof(int));

    for(size_t k = 0; k < nbImages * nbImages; ++k) s[k] = -1;

    get_optimal_cost_rec(0, nbImages - 1, m, s, cost, nbImages);

    /* Calcul du tableau "positions" de taille (nbImages). */
    size_t* positions = (size_t*)calloc(nbImages, sizeof(size_t));

    get_positions_rec(0, nbImages - 1, positions, s, nbImages);

    /* Destruction des tableaux locaux à la fonction. */
    free(cost);
    free(m);
    free(s);


    return positions;
}


PNMImage* packComic(const PNMImage** images, size_t nbImages, size_t comicWidth, size_t comicBorder, size_t comicInterval)
{
    /* Vérification de la validité des paramètres de la fonction. */
    if(!images) return NULL;
    if(comicWidth <= 2 * comicBorder) return NULL;

    /* Calcul du tableau "pos", des positions respectives des cases sur chaque ligne de l'image. */
    printf("Getting optimal positions... ");
    size_t* pos = wrapImages(images, nbImages, comicWidth, comicBorder, comicInterval);
    printf("DONE\n");

    /* Initialisation de l'image finale, sachant que toutes les cases font la même hauteur. */
    printf("Preparing comic... ");
    PNMImage* outputImage = createPNM(comicWidth, 2 * comicBorder + pos[nbImages - 1] * (images[0]->height + comicInterval) + images[0]->height);
    for(size_t i = 0; i < outputImage->width * outputImage->height; ++i) outputImage->data[i] = (PNMPixel){255, 255, 255};
    printf("DONE\n");

    /* Initialisation du tableau "costLine", des nombres respectifs de pixels à redimensionner pour chaque ligne, et du nombre de cases par ligne. */
    long* costLine = (long*)calloc(2 * (pos[nbImages - 1] + 1), sizeof(long));

    /* Calcul des valeurs de "costLine". */
    for(size_t i = 0; i <= pos[nbImages - 1]; ++i) costLine[i * 2] = comicWidth - 2 * comicBorder;
    for(size_t i = 0; i < nbImages; ++i)
    {
        costLine[pos[i] * 2] -= images[i]->width;
        costLine[pos[i] * 2 + 1] += 1;
    }
    for(size_t i = 0; i <= pos[nbImages - 1]; ++i) costLine[i * 2] -= comicInterval * (costLine[i * 2 + 1] - 1);

    /* Initialisation du tableau "cost", des nombres respectifs de pixels à redimensionner par case. */
    long* cost = (long*)calloc(nbImages, sizeof(long));

    /* Déclaration d'une variable "i_line" pour le calcul des valeurs de "cost". */
    size_t i_line = 0;

    /* Calcul des valeurs de "cost". */
    printf("Getting new sizes for images... ");
    for(size_t i = 0; i <= pos[nbImages - 1]; ++i)
    {
        for(size_t k = 0; k < costLine[i * 2 + 1]; ++k)
        {
            cost[i_line + k] = costLine[i * 2];
        }
        for(size_t k = 0; k < costLine[i * 2 + 1]; ++k)
        {
            cost[i_line + k] /= (costLine[i * 2 + 1] - (long)k);
            for(size_t l = k + 1; l < costLine[i * 2 + 1]; ++l)
            {
                cost[i_line + l] -= cost[i_line + k];
            }
        }
        i_line += costLine[i * 2 + 1];
    }
    printf("DONE\n");

    /* Initialisation du tableau "tid", des ID des threads de redimensionnement des images. */
    pthread_t* tid = (pthread_t*)malloc(nbImages * sizeof(pthread_t));

    /* Initialisation du tableau "arg", des arguments des threads de redimensionnement des images. */
    arg_t* arg = (arg_t*)malloc(nbImages * sizeof(arg_t));
    for(size_t i = 0; i < nbImages; ++i) arg[i] = (arg_t){images[i], cost[i]};

    /* Initialisation du tableau "modifiedImages", des images redimensionnées. */
    PNMImage** modifiedImages = (PNMImage**)malloc(nbImages * sizeof(PNMImage*));

    /* Création des threads de redimensionnement des images. */
    printf("Creating threads to resize images... ");
    for(size_t i = 0; i < nbImages; ++i)
    {
        printf("\rCreating threads to resize images... (%zu / %zu)", i + 1, nbImages);
        fflush(stdout);
        pthread_create(tid + i, NULL, &thread_modifyImageWidth, (void*)(arg + i));
    }
    printf("\rCreating threads to resize images... DONE          \n");

    /*Réception des images redimensionnées depuis les threads. */
    printf("Receiving resized images from threads... ");
    for(size_t i = 0; i < nbImages; ++i)
    {
        printf("\rReceiving resized images from threads... (%zu / %zu)", i + 1, nbImages);
        fflush(stdout);
        pthread_join(tid[i], (void**)(modifiedImages + i));
    }
    printf("\rReceiving resized images from threads... DONE          \n");

    /* Déclaration de variables pour l'écriture des cases sur l'image. */
    size_t h = 0;
    size_t w = 0;

    /* Écriture des cases sur l'image. */
    printf("Packing images... ");
    for(size_t i = 0; i < nbImages; ++i)
    {
        printf("\rPacking images... (%zu / %zu)", i + 1, nbImages);
        fflush(stdout);
        h = comicBorder + pos[i] * (images[0]->height + comicInterval);
        if(i == 0 || pos[i - 1] < pos[i]) w = comicBorder;
        else w += modifiedImages[i - 1]->width + comicInterval;

        for(size_t j = 0; j < modifiedImages[i]->height; ++j)
        {
            for(size_t k = 0; k < modifiedImages[i]->width; ++k)
            {
                outputImage->data[(h + j) * outputImage->width + (w + k)] = modifiedImages[i]->data[j * modifiedImages[i]->width + k];
            }
        }
    }
    printf("\rPacking images... DONE            \n");

    /* Destruction des tableaux locaux à la fonction. */
    free(pos);
    free(costLine);
    free(cost);

    for(size_t i = 0; i < nbImages; ++i) freePNM(modifiedImages[i]);
    free(modifiedImages);

    free(tid);
    free(arg);


    printf("Comic packed ! \n");
    return outputImage;
}
