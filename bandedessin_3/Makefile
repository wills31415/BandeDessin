all: Comic SeamCarving test


Comic: mainComic.o Comic.o SeamCarving.o PNM.o
	gcc -o Comic mainComic.o Comic.o SeamCarving.o PNM.o -pthread

mainComic.o: mainComic.c
	gcc -o mainComic.o -c mainComic.c

Comic.o: Comic.c
	gcc -o Comic.o -c Comic.c


SeamCarving: mainSeamCarving.o SeamCarving.o PNM.o
	gcc -o SeamCarving mainSeamCarving.o SeamCarving.o PNM.o

mainSeamCarving.o: mainSeamCarving.c
	gcc -o mainSeamCarving.o -c mainSeamCarving.c

SeamCarving.o: SeamCarving.c
	gcc -o SeamCarving.o -c SeamCarving.c -pthread


test: mainTest.o Comic.o SeamCarving.o PNM.o
	gcc -o test mainTest.o Comic.o SeamCarving.o PNM.o -lm -pthread

mainTest.o: mainTest.c
	gcc -o mainTest.o -c mainTest.c


PNM.o: PNM.c
	gcc -o PNM.o -c PNM.c


clean:
	rm -f *.o Comic SeamCarving test *.pnm
