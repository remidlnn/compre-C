/*******************/
/* Strucutre Noeud */
/*******************/
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
