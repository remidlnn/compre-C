# Compression : mise en oeuvre du codage d'Huffman
Dans le cadre du cours langage C du master PISE, ce programme implémente le codage d'huffman pour compresser et décompresser tout type de fichier.

## Installation

Importer le package et l'installer avec le script `installer.sh` qui se charge d'appeler la commande `make` depuis le dossier `src`.

### Usage

```
./comp [OPTION] [ARGUMENT]

Options:
  -c            Pour compresser
  -d            Pour décompresser

Arguments:
  <cible>       Nom du fichier cible
  <destination> Nom du fichier destination

Retour:
  0 : OK
  1 : Erreur dans la commande
  2 : Erreur pendant la lecture du fichier
  3 : Erreur pendant l'écriture du fichier
  4 : Compression inutile (fichier en soritie plus lourd)

Exemples:

./comp -c fichier.txt fichier.txt.comp
./comp -d fichier.txt.comp fichier.txt
```

### Test
Lancer le script bash, il faudra peut-être le rendre exécutable `chmod +x lancer_test`).
```
./lancer_test.sh
```
 Ce script va compresser/décompresser un ensemble de fichier de test (`data_test/*`) et enregistrer le résultat de l'exécution dans un fichier `resultat_test.txt`

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
