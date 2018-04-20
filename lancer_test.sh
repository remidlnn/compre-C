#!/bin/bash
# Vider le dossier output des précédentes exécutions
rm output/*
# Ajouter une ligne de séparation dans résultat de test.
echo "|------------------------------------- $(date)  ------------------------------------------------|" >> resultat_test.txt
# Joue (compression, décompression) l'ensemble des données de test
FICHIERS=`ls data_test/`
for FICHIER in $FICHIERS
do
    ./comp -c data_test/$FICHIER output/$FICHIER.comp >> resultat_test.txt
    ./comp -d output/$FICHIER.comp output/$FICHIER >> resultat_test.txt
    # Vérifie que les fichiers sont identiques 
    diff data_test/$FICHIER output/$FICHIER >> resultat_test.txt
done
# Affiche les résultats
cat resultat_test.txt
