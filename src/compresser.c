#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "noeud.h"
#include "affichage.h"

/******************************************************************************/
/*                           Constantes                                       */
/******************************************************************************/
// #define DEBUG
#define OPTION_COMP "-c"
#define OPTION_DECOMP "-d"
#define TAILLE_MORCEAU 1000000

/******************************************************************************/
/*                      Déclaration préalable des fonctions                   */
/******************************************************************************/
void estimer_compression(int * p_nb_bit, unsigned int * p_nb_octet_comp);
int calculer_frequences(char * nom_src, unsigned int * tab_freq, unsigned int * p_nb_octet_decomp);
int compresser(char * nom_dst, char * nom_src);
int importer_frequence(char * nom_fichier);
int decompresser(char * nom_src, char * nom_dst);
void construire_tab_p_noeuds();
void construire_arbre();
int verifier_fichier(char * nom_fichier);
void o_trier_tab(struct Noeud * tab[], int n);

/******************************************************************************/
/*                        Variables globales                                  */
/******************************************************************************/
unsigned int tab_frequences[256] = {0}   ; // Contiendra les fréquences des 256 valeurs que peut prendre un octet. Initialisé à 0.
unsigned int nb_octet_decomp=0           ; // Nb d'octets du fichier décompressé
unsigned int nb_octet_comp=0             ; // Nb d'octets du ficheir compressé
int nb_bit=0                             ; // Nb de bit significatif du dernier octet compressé
struct Noeud * p_racine = NULL           ; // Correspond à la racine de l'arbre
struct Noeud * tab_p_noeuds[256] = {NULL}; // Tableau de pointeurs vers les noeuds
int nb_noeuds = 0                        ; // Nb de noeud dans le tableau de pointeurs de noeuds

/*******************************************************************************
    Fonction principale

    Retourne :
    - 0 si ok
    - 1 erreur dans la commande
    - 2 erreur lecture
    - 3 erreur ecriture
    - 4 erreur compression inutile

*******************************************************************************/
int main(int argc, char *argv[])
{
    int retour; // Pour stocker et pouvoir tester   le retour d'une fonction
    // Vérifier que les arguments passés en ligne de commande sont correctes
    if (argc != 4||(strcmp(argv[1],OPTION_COMP)&&strcmp(argv[1],OPTION_DECOMP)))
    {
        return afficher_erreur(1, NULL);
    }

    // Commande lancée avec l'option de compression.
    if (strcmp(argv[1],OPTION_COMP) == 0)
    {
        // Calculer les fréquences
        if (calculer_frequences(argv[2], tab_frequences, &nb_octet_decomp) != 0)
        {
            return afficher_erreur(2, argv[2]);
        }

        // Construire l'arbre à partir des fréquences
        construire_tab_p_noeuds();
        construire_arbre();

        // Vérifier que la compression est rentable
        estimer_compression(&nb_bit, &nb_octet_comp);
        if (nb_octet_comp + (sizeof(unsigned int)*257) > nb_octet_decomp)
        {
            return afficher_erreur(4, argv[2]);
        }


        // Compresser en vérifiant le retour
        if ( (retour = compresser(argv[3], argv[2])) != 0)
        {
            switch (retour) {
                case 2: return afficher_erreur(2, argv[2]); // erreur de lecture
                case 3: return afficher_erreur(3, argv[3]); // erreur d'écriture
            }
        }

        return afficher_succes(1, argv[2], argv[3], nb_octet_comp, nb_octet_decomp);
    }

    // Commande lancée avec l'option de décompression.
    if (strcmp(argv[1],OPTION_DECOMP) == 0)
    {
        // Vérifier que le fichier src soit bien un fichier créé par le logiciel
        if ((retour = verifier_fichier(argv[2])) != 0)
        {
            return afficher_erreur(retour, argv[2]);
        }

        if (importer_frequence(argv[2]) != 0) // Met aussi à jour nb_octet_decomp
        {
            return afficher_erreur(2, argv[2]);
        }
        #ifdef DEBUG
            afficher_frequences(tab_frequences);
        #endif

        construire_tab_p_noeuds();
        construire_arbre();

        estimer_compression(&nb_bit, &nb_octet_comp); // Nécessaire pour connaitre nb_bit et nb_octet_comp
        // Décompresser en vérifiant le retour
        if ( (retour = decompresser(argv[2], argv[3])) != 0)
        {
            switch (retour) {
                case 2: return afficher_erreur(2, argv[2]); // cas d'erreur de lecture
                case 3: return afficher_erreur(3, argv[3]); // cas d'erreur d'écriture
            }
        }

        return afficher_succes(2, argv[2], argv[3], nb_octet_comp, nb_octet_decomp);
    }
}

/*******************************************************************************
    Calculer les fréquences en lisant chaque octet du fichier source. Met à jour
    la variable globale nb_octet_decomp.

    char * nom_src                      : nom du fichier source
    int * tab_freq                      : pointeur vers un tableau de fréquences
    unsigned int * p_nb_octet_decomp    : pointeur vers variable globale nb_octet_decomp

*******************************************************************************/
int calculer_frequences(char * nom_src, unsigned int * tab_freq, unsigned int * p_nb_octet_decomp)
{
    #ifdef DEBUG
        printf("--> calculer_frequences\n");
    #endif

    int retour; // Pour tester le retour de fgetc()
    unsigned char octet;
    FILE * fsrc;

    (*p_nb_octet_decomp) = 0; // Initialiser le nombre d'octet à 0.

    // Ouvrir proprement le fichier en lecture
    fsrc = fopen(nom_src, "rb");
    if (fsrc == NULL)
    {
        return 2;
    }

    while ((retour = fgetc(fsrc)) != EOF) // Tant qu'on a pas atteint la fin du fichier
    {
        octet = (unsigned char) retour; // Caster car fgetc retourne un int !
        tab_freq[octet]++;              // Incrémente la fréquence correspondant à l'octet
        (*p_nb_octet_decomp) += 1;      // Incrémenter l'ensemble des octets décomp.
    }

    #ifdef DEBUG
        afficher_frequences(tab_freq);
    #endif

    return 0;
}

/*******************************************************************************
    Compresser un tableau d'octet à partir d'un arbre en lisant le fichier
    source caractère par caractère et en écrivant le fichier destination.

    char * nom_dst                              : nom du fichier de destination
    char * nom_src                              : nom du fichier de destination

    (globale) unsigned int nb_octet_decomp      : nb octet decomp
    (globale) struct Noeud * tab_p_noeuds[256]  : tableau d'adresse de noeud

    Retourne :
    0 si ok
    2 si pb de lecture (sur source)
    3 si pb d'écriture (sur destination)

*******************************************************************************/
int compresser(char * nom_dst, char * nom_src)
{
    #ifdef DEBUG
        printf("--> compresser\n");
    #endif

    nb_bit = 1; // nb_bit est un curseur qui indique le bit où on en est dans l'octet
    unsigned char octet_a_ecrire=0;
    unsigned char octet_lu;
    struct Noeud * p_n;
    int i=0,j=0;
    unsigned int taille_morceau_a_lire;
    unsigned char morceau_a_lire[TAILLE_MORCEAU];
    unsigned char morceau_a_ecrire[TAILLE_MORCEAU];
    unsigned char * dernier_morceau_a_ecrire; // sera redimenssionné dynamiquement
    FILE * fdst;
    FILE * fsrc;

    // Ouvrire le fichier de destination (compressé)
    fdst = fopen(nom_dst, "wb");
    if (fdst == NULL)
    {
        return 3;
    }

    // Ouvrire le fichier source (à compresser)
    fsrc = fopen(nom_src, "rb");
    if (fsrc == NULL)
    {
        return 2;
    }

    // Ecrire le nb_octet_decomp (nécessaire pour vérifier le fichier)
    fwrite(&nb_octet_decomp, sizeof(unsigned int), 1, fdst);

    // Ecrire le tableau de 256 fréquences
    fwrite(tab_frequences, sizeof(unsigned int), 256, fdst);

    // Lire le premier morceau
    if (nb_octet_decomp % TAILLE_MORCEAU)
    {
        taille_morceau_a_lire = nb_octet_decomp % TAILLE_MORCEAU;
    }
    else
    {
        taille_morceau_a_lire = TAILLE_MORCEAU;
    }

    // Tant qu'il reste des morceaux à lire
    while (fread(morceau_a_lire, sizeof(unsigned char), taille_morceau_a_lire, fsrc) == taille_morceau_a_lire)
    {
        // printf("MORCEAU\n"); // debug

        // Boucle sur l'ensemble des octets du morceau à compresser
        for (i=0; i < taille_morceau_a_lire; i++)
        {
            // Affecter à p_n le noeud correspondant à la valeur de l'octet
            octet_lu = morceau_a_lire[i];
            // printf("octet_lu : %c\n", octet_lu); // debug
            p_n = tab_p_noeuds[octet_lu];

             // Tant qu'on n'a pas atteint la racine
            while (p_n->precedent != NULL)
            {
                // Si le noeud correspond au noeud descendant droite de son noeud précédent.
                // Autrement dit : si les pointeurs pointent vers la même adresse.
                // Sinon correspond au noeud descendant gauche
                // rien dans le else car le >> 1 ajoute un 0 !
                if (p_n->precedent->droite == p_n)
                {
                    octet_a_ecrire += 1;
                }

                // Ecrire l'octet s'il est complété (8 bits écrits)
                if (nb_bit == 8)
                {
                    // ajoute le nouvel octet et incrémente le compteur
                    morceau_a_ecrire[j++] = octet_a_ecrire;
                    octet_a_ecrire = 0                    ;
                    nb_bit = 0                            ;
                }

                // Ecrire le morceau à ecrire s'il est complet
                if (j == TAILLE_MORCEAU)
                {
                    fwrite(morceau_a_ecrire, sizeof(unsigned char), TAILLE_MORCEAU, fdst);
                    j=0; // Repartir à 0
                }

                // Décaller les bits de 1 vers la gauche
                nb_bit++                            ;
                octet_a_ecrire = octet_a_ecrire << 1;

                // Remonter au noeud précédent
                p_n = p_n->precedent;
            }
        }
        // remettre la taille normale (différe de TAILLE_MORCEAU seulement au 1er tour)
        taille_morceau_a_lire = TAILLE_MORCEAU;
    }

    // Annuler le dernier tour de boucle au niveau de l'octet et ecrire le dernier octet
    octet_a_ecrire = octet_a_ecrire >> 1;
    morceau_a_ecrire[j++] = octet_a_ecrire;

    // Ecrire le dernier morceau qui n'est pas forcément complet
    // Copier le contenu du morceau dans le dernier_morceau redimenssionné
    dernier_morceau_a_ecrire = (unsigned char *) malloc(sizeof(unsigned char) * j);
    fwrite(morceau_a_ecrire, sizeof(unsigned char), j, fdst);

    // Fermer les fichiers
    fclose(fdst);
    fclose(fsrc);

    return 0;
}

/*******************************************************************************
    Importer les fréquences et le nombre d'octet décompressé depuis un fichier
    compressé.

    char * nom_fichier : nom du fichier

    Retourne :
    - 0 si ok
    - 1 si erreur

*******************************************************************************/
int importer_frequence(char * nom_fichier)
{
    #ifdef DEBUG
        printf("--> importer_frequence\n");
    #endif

    FILE *f1;
    f1 = fopen(nom_fichier, "rb");
    if (f1 == NULL)
    {
         return 1;
    }

    // Se placer au début des fréquences (passer le premier unsigned int)
    fread(&nb_octet_decomp, sizeof(unsigned int), 1, f1);

    // Lire les fréquences
    fread(tab_frequences, sizeof(unsigned int), 256, f1);

    return 0;
}

/*******************************************************************************
    Décompresser les octets d'un fichier source dans un fichier destination.
    Pour des raisons de performance il est nécessaire de passer par des buffers
    en lecture (morceau_a_lire) et en écriture (morceau_a_ecrire). Cela est du
    au fait qu'on écrit et lit les fichiers source et destination à l'envers.

    char * nom_src : nom du fichier source
    char * nom_dst : nom du fichier de destination

    (globale) unsigned int nb_octet_decomp      : nb octet decomp
    (globale) unsigned int nb_octet_comp        : nb octet comp
    (globale) int nb_bit                        : nb de bit significatif premier octet

    Retourne :
    0 si ok
    2 si pb de lecture (sur source)
    3 si pb d'écriture (sur destination)

*******************************************************************************/
int decompresser(char * nom_src, char * nom_dst)
{
    #ifdef DEBUG
        printf("--> decompresser\n");
    #endif

    unsigned char octet_lu;
    unsigned char morceau_a_lire[TAILLE_MORCEAU];
    unsigned char morceau_a_ecrire[TAILLE_MORCEAU];
    unsigned char * dernier_morceau;
    int taille_dernier_morceau;
    int i=0, j=TAILLE_MORCEAU, taille_morceau_a_lire;
    struct Noeud * p_n = p_racine; // Se positionner à la racine de l'arbre
    FILE * fsrc;
    FILE * fdst;

    // Ouvrire le fichier source (à décompresser) et se positioner à la fin.
    fsrc = fopen(nom_src, "rb");
    if (fsrc == NULL)
    {
        return 2;
    }

    // Lire le premier morceau (à partir de la fin)
    taille_morceau_a_lire=(nb_octet_comp % TAILLE_MORCEAU);
    fseek(fsrc, - taille_morceau_a_lire* sizeof(unsigned char), SEEK_END);
    fread(morceau_a_lire, sizeof(unsigned char), taille_morceau_a_lire, fsrc);
    fseek(fsrc, - taille_morceau_a_lire* sizeof(unsigned char), SEEK_CUR);

    // Ouvrire le fichier de destination (décompressé) et se positionne à la fin
    // qui est calculée par rapport à nb_octet_decomp.
    fdst = fopen(nom_dst, "wb");
    if (fdst == NULL)
    {
        return 3;
    }
    fseek(fdst, nb_octet_decomp * sizeof(unsigned char), SEEK_SET);

    // Boucler sur l'ensemble des octets du fichier compressé
    for (i; i<nb_octet_comp; i++)
    {
        // Lire un nouveau morceau si on a lu tous les octets du morceau_a_lire
        if (taille_morceau_a_lire == 0)
        {
            fseek(fsrc, - TAILLE_MORCEAU* sizeof(unsigned char), SEEK_CUR);
            fread(morceau_a_lire, sizeof(unsigned char), TAILLE_MORCEAU, fsrc);
            fseek(fsrc, - TAILLE_MORCEAU* sizeof(unsigned char), SEEK_CUR);
            taille_morceau_a_lire = TAILLE_MORCEAU;
        }
        // Lire un octet du morceau_a_lire
        octet_lu = morceau_a_lire[--taille_morceau_a_lire];
        // Tant qu'il reste des bits à lire dans l'octet lu
        while (nb_bit > 0)
        {
            // Descendre l'arbre en fonction de la valeur de l'octet le plus à droite
            if ((octet_lu % 2) != 0)
            {
                p_n = p_n->droite;
            }
            else
            {
                p_n = p_n->gauche;
            }

            // Ecrire l'octet si l'on a atteint une feuille
            if (p_n->gauche == NULL && p_n->droite == NULL)
            {
                // Ajouter l'octet à la fin du morceau
                morceau_a_ecrire[--j] = p_n->octet;
                // Ecrire le morceau s'il est complet
                if (j==0)
                {
                    // Ecrire le morceau à l'envers (remonter/écrire/remonter)
                    fseek(fdst, - sizeof(unsigned char)*TAILLE_MORCEAU, SEEK_CUR);
                    fwrite(morceau_a_ecrire, sizeof(unsigned char), TAILLE_MORCEAU, fdst) ;
                    fseek(fdst, - sizeof(unsigned char)*TAILLE_MORCEAU, SEEK_CUR);
                    j = TAILLE_MORCEAU;
                }
                p_n = p_racine; // Repartir de la racine
            }

            nb_bit--; // controle la boucle while

             // décaler les bits vers la droite
            octet_lu = octet_lu >> 1;
        }
        nb_bit = 8;
    }

    // Ecrire le dernier morceau qui n'est pas forcément complet
    taille_dernier_morceau = TAILLE_MORCEAU - j;
    // Copier le contenu du morceau dans le dernier_morceau redimenssionné
    dernier_morceau = (unsigned char *) malloc(sizeof(unsigned char) * taille_dernier_morceau);
    memcpy(dernier_morceau, morceau_a_ecrire + j, taille_dernier_morceau * sizeof(unsigned char));
     // Se positionner au début du fichier et écrire le dernier morceau
    fseek(fdst, 0, SEEK_SET);
    fwrite(dernier_morceau, sizeof(unsigned char), taille_dernier_morceau, fdst) ;

    // Fermer les fichiers
    fclose(fdst);
    fclose(fsrc);

    return 0;
}

/*******************************************************************************
    Remplir le tableau de pointeur de noeuds à partir des fréquences et stocker le
    nombre de noeud.

    (globale) unsigned int tab_frequences[256] : tableau de fréquences
    (globale) struct Noeud * tab_p_noeuds[256] : tableau d'adresse de noeud
    (globale) int nb_noeuds                    : nombre de noeud

*******************************************************************************/
void construire_tab_p_noeuds()
{
    #ifdef DEBUG
        printf("--> construire_tab_p_noeuds");
    #endif

    int i;
    struct Noeud * p_n;
    for (i=0; i < 256; i++)
    {
        if (tab_frequences[i] > 0)
        {
            // Créer un noeud à partir de l'élément i du tableau de fréquences.
            p_n = (struct Noeud *) malloc(sizeof(struct Noeud));
            p_n->octet = i                ;
            p_n->poids = tab_frequences[i];
            p_n->gauche = NULL            ;
            p_n->droite = NULL            ;
            p_n->precedent = NULL         ;

            // Enregister l'adresse du noeud dans le tableau de noeuds.
            tab_p_noeuds[i] = p_n;
            nb_noeuds++; // mettre à jour le nombre de noeuds
        }
    }
    #ifdef DEBUG
        afficher_tab_p_noeuds(tab_p_noeuds);
    #endif
}

/*******************************************************************************
    Construire l'arbre.
    (globale) struct Noeud * tab_p_noeuds[256] : tableau d'adresse de noeud
    (globale) int nb_noeuds                    : nombre de noeud

*******************************************************************************/
void construire_arbre()
{
    #ifdef DEBUG
        printf("--> construire_arbre\n");
    #endif
    int i,j=0, k=0, permutation;
    struct Noeud * p_n;
     // Copie de tab_p_noeuds qui ne contiendra que des noeuds non vides.
    struct Noeud * copy_tab_p_noeud[nb_noeuds];

    // Remplir la copy avec les noeuds non vide.
    for (i=0; i<256; i++)
    {
        // Si le noeud existe
        if (tab_p_noeuds[i] != NULL)
        {
            copy_tab_p_noeud[j++] = tab_p_noeuds[i];
        }
    }

    // Trier le tableau de noeud.
    o_trier_tab(copy_tab_p_noeud, nb_noeuds);

    // Chainer les noeuds du tableau pour créer l'arbre
    for (i=1; i < nb_noeuds; i++) // i=1 à cause du i-1 dans la suite
    {
        // Créer un nouveau noeud
        p_n = (struct Noeud *) malloc(sizeof(struct Noeud));
        // relier les deux noeuds les plus faibles vers le nouveau noeud.
        copy_tab_p_noeud[i-1]->precedent = p_n;
        copy_tab_p_noeud[i]->precedent = p_n  ;
        // relier le nouveau noeud vers les deux noeuds les plus faibles
        p_n->gauche = copy_tab_p_noeud[i-1]   ;
        p_n->droite = copy_tab_p_noeud[i]     ;
        // Calculer le nouveau poids du noeud.
        p_n->poids = copy_tab_p_noeud[i]->poids + copy_tab_p_noeud[i-1]->poids;
        // Remettre p_n dans le tableau.
        copy_tab_p_noeud[i] = p_n             ;

        o_trier_tab(copy_tab_p_noeud, nb_noeuds);
    }
    // Le noeud racine correspond aux derniers noeuds ajoutés dans le tableau = copy_tab_p_noeud[255]
    p_racine = copy_tab_p_noeud[nb_noeuds-1];
    p_racine->precedent = NULL;

    #ifdef DEBUG
        afficher_octet_code(tab_p_noeuds);
    #endif

}

/*******************************************************************************
    Estimer compression met à jour nb_bit et nb_octet_comp.

    int * p_nb_bit        : pointeur vers la variable globale nb_bit
    unsigned int * p_nb_octet_comp : pointeur vers la variable globale nb_octet_comp

*******************************************************************************/
void estimer_compression(int * p_nb_bit, unsigned int * p_nb_octet_comp)
{
    #ifdef DEBUG
        printf("--> estimer_compression\n");
    #endif

    int i, taille=0;
    unsigned int total_bit=0;
    struct Noeud * p_n;
    for (i=0; i<256; i++)
    {
        p_n = tab_p_noeuds[i];
        if (p_n != NULL)
        {
            while (p_n != NULL)
            {
                p_n = p_n->precedent;
                taille++;
            }
        }
        // Ajouter au total_bit, la taille d'un octet multipliée par sa fréquence
        total_bit += (--taille) * tab_frequences[i];
        taille = 0;
    }

    // Calculer l'ensemble des octets complétés
    *p_nb_octet_comp = total_bit/8 +1;
    // Calculer le nb_bit
    *p_nb_bit = total_bit % 8;

    #ifdef DEBUG
        printf("nb_octet_comp : %d\n", nb_octet_comp);
        printf("nb_bit : %d\n", nb_bit);
    #endif
}

/*******************************************************************************
    Vérifier qu'un fichier en entré pour décompression est bien un fichier
    issu du programme. Consiste à lire le premier unsigned int et à le comparer
    à la somme des fréquences (doit être égale !).

    char * nom_fichier : nom du fichier

*******************************************************************************/
int verifier_fichier(char * nom_fichier)
{
    unsigned int nb_octet_affiche, nb_octet_reel=0, taille_fichier,i, tab_freq[256];
    FILE * fsrc;
    fsrc = fopen(nom_fichier, "rb");
    if (fsrc == NULL)
    {
        return 2;
    }
    // Vérifier que le fichier fait au minima la taille du nb_octet_decomp + les 256 fréquences
    fseek(fsrc, 0, SEEK_END);
    taille_fichier = ftell(fsrc);
    if (taille_fichier < 257 * sizeof(unsigned int))
    {
        return 5;
    }
    // Sinon vérifier que la somme des fréquences vaut nb_octet_decomp
    else
    {
        fseek(fsrc, 0, SEEK_SET);
        fread(&nb_octet_affiche,sizeof(unsigned int), 1, fsrc);
        fread(tab_freq, sizeof(unsigned int), 256, fsrc);
        for (i=0; i< 256; i++)
        {
            nb_octet_reel += tab_freq[i];
        }
        if (nb_octet_reel == nb_octet_affiche)
        {
            return 0;
        }
        else
        {
            return 5;
        }
    }

    fclose(fsrc);
}

/*******************************************************************************
    Trier un tableau de pointeur de Noeud (trie par selection)

    struct Noeud * tab[] : tableau de pointeur vers des struct Noeud
    int n                : taille du tableau

*******************************************************************************/
void o_trier_tab(struct Noeud * tab[], int n)
{
    int i, j;
    struct Noeud * p_n;
    for (i=0; i<n-1; i++)
    {
        for (j=i+1; j<n; j++)
        {
            if (tab[i]->poids > tab[j]->poids)
            {
                p_n = tab[i]   ;
                tab[i] = tab[j];
                tab[j] = p_n   ;
            }
        }
    }
}
