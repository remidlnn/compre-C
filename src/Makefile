# Liste des objets
OBJETS= compresser.o affichage.o

# Compilation de l'exécutable
comp: $(OBJETS)
	cc -o comp $(OBJETS)

# Création des objets
compresser.o: compresser.c
	cc -c compresser.c

debug.o: affichage.c
	cc -c affichage.c
