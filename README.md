# Compression : mise en oeuvre du codage d'Huffman
Dans le cadre du cours langage C du master PISE, ce programme implémente le codage d'huffman pour compresser et décompresser tout type de fichier.

## Installation

Importer le package et l'installer avec le script `installer.sh` qui se charge d'appeler la commande `make` depuis le dossier `src`, de donner les droit d'exécution à l'exécutable et le déplace à la racine du dossier.
Il sera peut-être nécessaire de le rendre exécutable `chmod +x installer.sh`.

### Usage

```
./comp [OPTION] [ARGUMENT]

Options:
  -c            Pour compresser
  -d            Pour décompresser

Arguments:
<source>      Nom du fichier source à (dé)compresser en fonction de l’option
<destination> Nom du fichier destination, le résultat du traitement

Retour:
  0 : OK
  1 : Erreur dans la commande
  2 : Erreur pendant la lecture du fichier
  3 : Erreur pendant l'écriture du fichier
  4 : Compression inutile (fichier en soritie plus lourd)
  5 : Fichier à décompresser non valide (pas issu d'une compression du programme)

Exemples:

./comp -c fichier.txt fichier.txt.comp
./comp -d fichier.txt.comp fichier.txt
```

### Test
Le script bash `lancer_test.sh` se charge d'exécuter une série de test. Il sera peut-être nécessaire de le rendre exécutable `chmod +x lancer_test`.
```
./lancer_test.sh
```
 Ce script va compresser/décompresser un ensemble de fichier de test (`data_test/*`) pour tester les différentes retour (un succès et les 4 retour d'erreur possible).

### Ressources

Pour les fichiers de test (`data_test/*`)

- texte : [gutenberg.org](http://www.gutenberg.org/)
- image : [unsplash.com](https://unsplash.com)
- video : [videos.pexels.com](https://videos.pexels.com)

## Auteur

* **Rémi Dlnn**
* **Hong Trn**

Voir aussi la liste des [contributeurs](https://github.com/remidlnn/compression/graphs/contributors) qui ont participés au projet.

## License

Ce projet est sous licence MIT, voir le fichier [LICENSE.md](https://github.com/remidlnn/compression/blob/master/LICENCE.md) pour les détails.
