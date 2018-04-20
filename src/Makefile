OBJETS= compresser.o affichage.o

# compilation de l'executable
comp: $(OBJETS)
	cc -o comp $(OBJETS)

compresser.o: compresser.c
	cc -c compresser.c

debug.o: affichage.c
	cc -c affichage.c
