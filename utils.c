/*
 * ============================================================
 * Fichier     : utils.c
 * Description : Fonctions utilitaires : tirage de jetons,
 *               lecture/écriture des scores, pause clavier.
 * Auteurs     : Oumar Leyti Ndiaye , Mouhamed souley Kalilou
 * Date        : Hiver 2026
 * Sources IA  : Claude Sonnet (Anthropic) – génération du squelette
 * ============================================================
 */

#include "game.h"

/* ── Tirage aléatoire d'un jeton dans {-2,-1,0,1,2} ──────────────────── */
int token_draw(void)
{
    /* Tableau des valeurs possibles */
    static const int tokens[NUM_TOKENS] = {-2, -1, 0, 1, 2};
    return tokens[rand() % NUM_TOKENS];
}

/* ── Sauvegarde des scores dans SCORE_FILE ────────────────────────────── */
void score_save(const int scores[NUM_PLAYERS])
{
    /*Ouvrir le fichier en ecriture*/
    FILE *f = fopen(SCORE_FILE, "w");
    if (!f) {
        perror("score_save: impossible d ouvrir le fichier");
        return;
    }
    /* Ecrire les scores*/
    fprintf(f, "Joueur1=%d\nJoueur2=%d\n", scores[0], scores[1]);
    
    fclose(f);
    printf("[Score] Sauvegardé dans %s\n", SCORE_FILE);
}

/* ── Chargement des scores depuis SCORE_FILE ──────────────────────────── */
void score_load(int scores[NUM_PLAYERS])
{
    /* Initialiser les scores a 0 par defaut*/
    scores[0] = scores[1] = 0;

    /* Esayer d'ouvrir le fichier en lecture*/
    FILE *f = fopen(SCORE_FILE, "r");
    if (!f)  {
        printf("[score] Aucun fichier trouve, debut a 0.\n") ;
        return ;  /* fichier absent = premiere executions */
    }
    
    /* Lire les scores depuis le fichier*/
    fscanf(f, "Joueur1=%d\nJoueur2=%d\n", &scores[0], &scores[1]);

    fclose(f);
    printf("[Score] Chargé : J1=%d  J2=%d\n", scores[0], scores[1]);
}

/* ── Pause : attend que l'utilisateur appuie sur Entrée ──────────────── */
void wait_for_key(void)
{
    printf("  [Appuyez sur Entrée pour continuer...] ");
    fflush(stdout);

    /* Vider le buffer d'entrée puis lire un caractère */
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
