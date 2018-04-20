#!/bin/bash

# Installer le programme

# Supprimer l'exécutable s'il existe et les objects créés pendant la compilation
rm comp
rm src/*.o
# Se rendre de le dossier contenant les sources
cd src
# Créer l'executable
make
# Rendre comp exécutable
chmod +x comp
# déplacer l'executable à la racine du dossier
mv comp ../comp
