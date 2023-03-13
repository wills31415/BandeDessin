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
    printf("Creating threads to resize images... DONE          \n");

    /*Réception des images redimensionnées depuis les threads. */
    printf("Receiving resized images from threads... ");
    for(size_t i = 0; i < nbImages; ++i)
    {
        printf("\rReceiving resized images from threads... (%zu / %zu)", i + 1, nbImages);
        fflush(stdout);
        pthread_join(tid[i], (void**)(modifiedImages + i));
    }
    printf("Receiving resized images from threads... DONE          \n");

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
    int* costLine = (int*)calloc(2 * (pos[nbImages - 1] + 1), sizeof(int));

    /* Calcul des valeurs de "costLine". */
    for(size_t i = 0; i <= pos[nbImages - 1]; ++i) costLine[i * 2] = comicWidth - 2 * comicBorder;
    for(size_t i = 0; i < nbImages; ++i)
    {
        costLine[pos[i] * 2] -= images[i]->width;
        costLine[pos[i] * 2 + 1] += 1;
    }
    for(size_t i = 0; i <= pos[nbImages - 1]; ++i) costLine[i * 2] -= comicInterval * (costLine[i * 2 + 1] - 1);

    /* Initialisation du tableau "cost", des nombres respectifs de pixels à redimensionner par case. */
    int* cost = (int*)calloc(nbImages, sizeof(int));

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
            cost[i_line + k] /= (costLine[i * 2 + 1] - (int)k);
            for(size_t l = k + 1; l < costLine[i * 2 + 1]; ++l)
            {
                cost[i_line + l] -= cost[i_line + k];
            }
        }
        i_line += costLine[i * 2 + 1];
    }
    printf("DONE\n");

    /* Initialisation du tableau "modifiedImages", des images redimensionnées. */
    PNMImage** modifiedImages = (PNMImage**)malloc(nbImages * sizeof(PNMImage*));

    /* Calcul des images redimensionnées. */
    printf("Resizing images... ");
    for(size_t i = 0; i < nbImages; ++i)
    {
        printf("\rResizing images... (%zu / %zu)", i + 1, nbImages);
        fflush(stdout);
        if(cost[i] <= 0) modifiedImages[i] = reduceImageWidth(images[i], (size_t)(-cost[i]));
        else modifiedImages[i] = increaseImageWidth(images[i], (size_t)(cost[i]));
    }
    printf("\rResizing images... DONE            \n");

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

    printf("Comic packed ! \n");
    return outputImage;
}
