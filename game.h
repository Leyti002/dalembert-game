/*
 * ============================================================
 * Fichier     : game.h
 * Description : Définitions communes du jeu de d'Alembert.
 *               Contient les constantes, structures et
 *               prototypes partagés entre tous les modules.
 * Auteurs     : Oumar Leyti Ndiaye , Mouhamed souley Kalilou
 * Date        : Hiver 2026
 * Sources IA  : Claude Sonnet (Anthropic) – génération du squelette
 * ============================================================
 */

#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>

/* ── Dimensions de la grille ── */
#define GRID_SIZE   11          /* 11x11 cases                     */
#define CENTER       6          /* case centrale (1-indexée)       */
#define WIN_LOW      1          /* case victoire coin bas-gauche   */
#define WIN_HIGH    11          /* case victoire coin haut-droit   */

/* ── Jetons disponibles pour chaque tirage ── */
#define TOKEN_MIN   -2
#define TOKEN_MAX    2
#define NUM_TOKENS    5         /* {-2,-1,0,1,2}                  */

/* ── Nombre de joueurs ── */
#define NUM_PLAYERS  2

/* ── Fichier de scores ── */
#define SCORE_FILE  "scores.txt"

/* ── Cases marquées X sur la grille (coordonnées 1-indexées) ── */
/* À ajuster selon la Figure 1 du sujet                          */
typedef struct { int x; int y; } Point;

static const Point X_CELLS[] = {
    {2, 4}, {3, 9}, {6, 8}, {7, 5}, {6, 2}, {10, 2}, {9, 10}
};
#define NUM_X_CELLS  (int)(sizeof(X_CELLS)/sizeof(X_CELLS[0]))

/* ── Message échangé entre processus fils via pipe ── */
typedef struct {
    int player_id;   /* 0 ou 1                               */
    int new_x;       /* nouvelle position x du pion          */
    int new_y;       /* nouvelle position y du pion          */
    int game_over;   /* 1 si la partie est terminée          */
    int winner;      /* id du gagnant (-1 si pas terminé)   */
    int natural ; 
} MoveMsg;

/* ── État du jeu partagé dans chaque processus fils ── */
typedef struct {
    Point pos[NUM_PLAYERS];  /* positions courantes des deux pions */
    int   scores[NUM_PLAYERS];
    int   current_player;
} GameState;

/* ── Prototypes ── */

/* grid.c */

void grid_display(const GameState *gs);
int  grid_is_x_cell(int x, int y);
int  grid_in_bounds(int x, int y);

/* player.c */
void player_take_turn(GameState *gs, int player_id,
                      int fd_write, int fd_read __attribute__((unused)));

/* utils.c */
int  token_draw(void);
void score_save(const int scores[NUM_PLAYERS]);
void score_load(int scores[NUM_PLAYERS]);
void wait_for_key(void);

#endif /* GAME_H */
