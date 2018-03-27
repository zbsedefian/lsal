# make file
all: filegrep lsal lsal2
filegrep: filegrep.c
	gcc -o filegrep filegrep.c
lsal: lsal.c
	gcc lsal.c -lm -o lsal
lsal2: lsal2.c
	gcc lsal2.c -lm -o lsal2
clean:
	rm filegrep lsal lsal2
