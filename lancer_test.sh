#!/bin/bash

# Vider le dossier output des précédentes exécutions
rm output/*

echo "------------------------------------------------------------------"
echo "---- Teste une série de compression/décompression en succès 1 ----"
echo "------------------------------------------------------------------"
FICHIERS=`ls data_test/succes1`
for FICHIER in $FICHIERS
do
    ./comp -c data_test/succes1/$FICHIER output/$FICHIER.comp
    ./comp -d output/$FICHIER.comp output/$FICHIER
    # Vérifie que les fichiers sont identiques
    diff data_test/succes1/$FICHIER output/$FICHIER
done

echo "-----------------------------------------------"
echo "----------- Test en erreur 2  (lecture)--------"
echo "-----------------------------------------------"
./comp -c data_test/fichier_inexistant.txt output/fichier.comp
./comp -d data_test/fichier_inexistant.txt output/fichier.txt

echo "-------------------------------------"
echo "---- Test en erreur 3 (ecriture) ----"
echo "-------------------------------------"
# le dossier erreur3 contient un fichier sur le quel on n'a pas le droit décrire
./comp -c data_test/succes1/i_4.jpg data_test/erreur3/fichier.comp

echo "------------------------------------------------------------------"
echo "---- Teste une série de compression/décompression en erreur 4 ----"
echo "------------------------------------------------------------------"
# Erreur 4 correspond à des compression qui rendrait le fichier plus lourd qu'à l'origine
FICHIERS=`ls data_test/erreur4`
for FICHIER in $FICHIERS
do
    ./comp -c data_test/erreur4/$FICHIER output/$FICHIER.comp
done

echo "----------------------------------------------------------------------------"
echo "-- Teste erreur 5 (decompression d'un fichier non issu d'une compression) --"
echo "----------------------------------------------------------------------------"
# On va essayer de décompresser un ensemble de fichier qui ne sont pas le résultat
# d'une compression (les fichiers contenu dans le dossier succes1)
FICHIERS=`ls data_test/succes1`
for FICHIER in $FICHIERS
do
    ./comp -d data_test/succes1/$FICHIER output/$FICHIER.comp
done

echo "--------------------------------------"
echo "------ Test erreur d'arguments  ------"
echo "--------------------------------------"
# Doit afficher l'aide
# Sans argument
./comp
# Avec 1 argument manquant
./comp -c data_test/t_small.txt
# Avec option inexistante
./comp -a data_test/t_small.txt output/t_small.comp
# Avec option en trop
./comp -a data_test/t_small.txt output/t_small.comp entrop
