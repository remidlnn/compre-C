#include <stdio.h>
#include <string.h>

#ifndef STRUCT_NOEUD_H
#define STRUCT_NOEUD_H

struct Noeud {
    struct Noeud *precedent;
    unsigned char octet     ;
    int poids               ;
    struct Noeud *gauche    ;
    struct Noeud *droite    ;
};
#endif


/*******************************************************************************
    Afficher les fréquences :
    int * tab_freq : tableau des 256 fréquences
*******************************************************************************/
void afficher_frequences(unsigned int * tab_freq)
{
    int i;
    printf("------------------\n");
    printf("- tab frequences -\n");
    printf("------------------\n");
    printf("octet | frequences\n");
    printf("------------------\n");
    for (i=0; i<256; i++)
    {
        if (tab_freq[i] != 0)
        {
            printf("%5d | %10d\n", i, tab_freq[i]);
        }
    }
    printf("\n");
}


/*******************************************************************************
    Afficher le tableau de pointeur de noeud :
    struct Noeud * tabè[256] : pointeur vers tableau de 256 adresses de noeud

*******************************************************************************/
void afficher_tab_p_noeuds(struct Noeud * tab_p_noeuds[256])
{
    int i;
    printf("-----------------------------------\n");
    printf("-- tableau de pointeur de noeud  --\n");
    printf("-----------------------------------\n");
    printf("octet     | frequences             \n");
    printf("-----------------------------------\n");
    for (i=0; i<256; i++)
    {
        if (tab_p_noeuds[i] != NULL) // Possible qu'il n'y ait pas de noeud (fréquence de l'octet = 0)
        {
            printf("%9d | %23d\n", tab_p_noeuds[i]->octet, tab_p_noeuds[i]->poids);
        }
    }
    printf("\n");
}

/*******************************************************************************
    Afficher l'ensemble des octets du dictionnaire de neoud au format binaire
    struct Noeud * tab_p_noeuds[256] : pointeur vers tableau de 256 adresses de noeud

*******************************************************************************/
void afficher_octet_code(struct Noeud * tab_p_noeuds[256])
{
    int i;
    struct Noeud * p_noeud;
    printf("--------------------\n");
    printf("--- octets codés ---\n");
    printf("--------------------\n");
    printf("octet | valeur codée\n");
    printf("--------------------\n");
    for (int i=0; i<256; i++)
    {
        if (tab_p_noeuds[i] != NULL)
        {
            p_noeud = tab_p_noeuds[i];
            printf("%5d | ", p_noeud->octet);
            while (p_noeud->precedent != NULL)
            {
                if (p_noeud->precedent->droite == p_noeud) printf("1");
                else printf("0");
                p_noeud = p_noeud->precedent;
            }
            printf("\n");
        }
    }
}

/*******************************************************************************
    Afficher l'aide à l'utilisation.

*******************************************************************************/
void afficher_aide()
{
    printf("---------- Aide à l'utilisation ----------\n");
    printf("comp [OPTION] [ARGUMENT]\n\n")                ;
    printf("Options\n")                                   ;
    printf("    -c          : pour compresser\n")         ;
    printf("    -d          : pour décompresser\n")       ;
    printf("Argument\n")                                  ;
    printf("    cible       : nom du fichier\n")          ;
    printf("    destination : nom du fichier\n")          ;
}

/*******************************************************************************
    Afficher le resultat d'une éxecution réussi (compression ou decompression)
    et retourne la valeur de succès

    int option                   : 1 pour comp 2 pour decomp
    char * src                   : nom fichier à compresser
    char * dst                   : nom fichier compressé
    unsigned int nb_octet_comp   : nb octets compressés
    unsigned int nb_octet_decomp : nb octets déompressés

*******************************************************************************/
int afficher_succes(int option, char * src, char * dst, unsigned int nb_octet_comp, unsigned int nb_octet_decomp)
{
    float taux;
    unsigned int taille_info = sizeof(int)*257; // 256 frequences + 1 nb_octet_decomp
    char option_char[14];

    // Affecte le bon mot à la valeur numérique d'option
    switch (option)
    {
        case 1:
            strcpy(option_char, "compression");
            taux = ( (nb_octet_comp + taille_info) / (float) nb_octet_decomp ) * 100;
            printf("1 Succès : %13s de %30s [%10d] vers %30s [%10d] [%7.2f%%]\n", option_char, src, nb_octet_decomp, dst, nb_octet_comp + taille_info, taux);
            break;
        case 2:
            strcpy(option_char, "decompression");
            printf("1 Succès : %13s de %30s [%10d] vers %30s [%10d]\n", option_char, src, nb_octet_comp  + taille_info, dst, nb_octet_decomp);
            break;
        default:
            break;
    }
    return 0;
}

/*******************************************************************************
    Afficher le resultat d'une éxecution en erreur et retourne la valeur de
    l'erreur

    int erreur_num     : numéro de l'erreur (de 1 à 4)
    char * nom_fichier : nom fichier à compresser

*******************************************************************************/
int afficher_erreur(int erreur_num, char * nom_fichier)
{
    switch (erreur_num) {
        case 1 :
            // Nécessite de passer un nom_fichier fictif qui ne sera pas utilisé
            afficher_aide();
            return 1;
        case 2 :
            printf("2 Erreur : impossible de lire %s\n", nom_fichier);
            return 2;
        case 3 :
            printf("3 Erreur : impossible d'écrire %s\n", nom_fichier);
            return 3;
        case 4 :
            printf("4 Erreur : la compression ne serait pas efficace pour %s\n", nom_fichier);
            return 4;
        case 5 :
            printf("5 Erreur : le fichier %s n'est pas un fichier valide\n", nom_fichier);
            return 5;
        default:
            return 0; // améliorer gestion d'erreur dans erreur_num
    }
}
