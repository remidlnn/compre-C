void afficher_frequences(unsigned int * tab_freq);
void afficher_tab_p_noeuds(struct Noeud * dico[256]);
void afficher_octet_code(struct Noeud * dico[256]);
void afficher_aide();
int afficher_erreur(int erreur_num, char * nom_fichier);
int afficher_succes(int option, char * src, char * dst, unsigned int nb_octet_comp, unsigned int nb_octet_decomp);
