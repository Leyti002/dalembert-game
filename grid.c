/*
 * ============================================================
 * Fichier     : grid.c
 * Description : Gestion et affichage de la grille 11x11 du
 *               jeu de d'Alembert en mode texte (TUI).
 *               Fonctions : initialisation, dessin, tests.
 * Auteurs     : Oumar Leyti Ndiaye , Mouhamed souley Kalilou
 * Date        : Hiver 2026
 * Sources IA  : Claude Sonnet (Anthropic) – génération du squelette
 * ============================================================
 */

#include "game.h"

/* ── Affiche la grille avec les pions et les cases X ──────────────────── */
/*
 * Représentation textuelle :
 *   _  : case vide
 *   X  : case piège
 *   1  : pion du joueur 1
 *   2  : pion du joueur 2
 *   *  : les deux pions sur la même case
 *
 * La grille est affichée ligne 11 (haut) → ligne 1 (bas).
 */
void grid_display(const GameState *gs)
{
    /* TODO : optionnel – effacer l'écran avant de redessiner */
    /* printf("\033[2J\033[H"); */

    printf("\n  ╔");
    for (int c = 0; c < GRID_SIZE; c++) printf("═══");
    printf("╗\n");

    /*Parcours de chaque ligne de haut (11) vers le bas (1)*/
    for (int row = GRID_SIZE; row >= 1; row--) {
        printf("%2d║", row); // numero de ligne a gauche
        for (int col = 1; col <= GRID_SIZE; col++) { // parcours de chaque colonne

            /* Déterminer ce qui occupe la case (col, row) */
            int p1_here = (gs->pos[0].x == col && gs->pos[0].y == row);
            int p2_here = (gs->pos[1].x == col && gs->pos[1].y == row);
            int is_x    = grid_is_x_cell(col, row);

            /* Afficher le bon symbole */
            if (p1_here && p2_here)  printf(" * ");
            else if (p1_here)        printf(" 1 ");
            else if (p2_here)        printf(" 2 ");
            else if (is_x)           printf(" X ");
            else                     printf(" . ");
        }
        printf("║\n");
    }
    // Ligne du bas 
    printf("  ╚");
    for (int c = 0; c < GRID_SIZE; c++) printf("═══");
    printf("╝\n");

    /* Numéros de colonnes */
    printf("   ");
    for (int col = 1; col <= GRID_SIZE; col++) printf("%2d ", col);
    printf("\n");

    /* Positions des pions */
    printf("  Joueur 1 → (%d,%d)   Joueur 2 → (%d,%d)\n",
           gs->pos[0].x, gs->pos[0].y,
           gs->pos[1].x, gs->pos[1].y);
    printf("  Scores : J1=%d  J2=%d\n\n",
           gs->scores[0], gs->scores[1]);
}

/* ── Retourne 1 si (x,y) est une case X, 0 sinon ─────────────────────── */
int grid_is_x_cell(int x, int y)
{
    /* Parcourir toutes les caes X definies dans game.h*/
    for (int i = 0; i < NUM_X_CELLS; i++) {
        if (X_CELLS[i].x == x && X_CELLS[i].y == y)
            return 1; // Case X trouvee
    }
    return 0; // Pas une case X
}

/* ── Retourne 1 si (x,y) est dans la grille [1..11]x[1..11] ──────────── */
int grid_in_bounds(int x, int y)
{
    return (x >= 1 && x <= GRID_SIZE && y >= 1 && y <= GRID_SIZE);
}
