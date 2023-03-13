# BandeDessin

Ce projet a pour but principal de générer une planche de bande dessinée à partir d'une liste d'images numérotées fournie en entrée. L'algorithme de génération de décompose en deux étapes indépendantes.

  La première étape consiste à placer les images de manière optimale sur la planche. En effet, les images n'ont pas toutes la même taille. Il s'agit alors de minimiser les espaces vides entre ces dernières, sacnaht qu'elles doivent être disposées dans l'ordre. De plus, les images peuvent être redimensionnées dans le sens de la largeur (la hauteur reste fixe). Ainsi à cette étape, l'algorithme doit également calculer la largeur optimale de chaque image, en plus de sa position sur la planche.
Cette étape de minimisation utilise, entre autres, la programmation dynamique via des tables de mémoïsation.

  La deuxième étape consiste simplement à effectuer les éventuels redimensionnements des images, et à générer la planche finale qui sera retournée par le programme. Pour modifier la largeur des images, le programme utilise l'algorithme du "Seam Carving" (https://fr.wikipedia.org/wiki/Seam_carving). Il est important de noter que cette étape est particulièrement propice à la parallélisation. Vous retrouverez ainsi, en plus de la version séquentielle, une version parallélisée de ce programme, implémentée à l'aide de la bibliothèque "pthread" du standard C.

Enfin, sachez que vous trouverez dans la branche principale de ce projet, deux fichiers PDF qui vous donneront plus d'informations sur le projet en lui-même ("main/projet.pdf"), ainsi que sur l'algorithme du "Seam Carving" ("main/seamcarving.pdf").
