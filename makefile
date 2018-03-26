# make file
all: filegrep.o lsal.o lsal2.o
	gcc -o filegrep.o lsal.o lsal2.o
filegrep.o: filegrep.c
	gcc -c filegrep.c
lsal.o: lsal.c
	gcc -c lsal.c
lsal2.o: lsal2.c
	gcc -c lsal2.c
clean:
	rm filegrep lsal lsal2
