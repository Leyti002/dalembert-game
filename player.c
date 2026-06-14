/*
 * ============================================================
 * Fichier      : player.c
 * Description  : Logique complète d'un tour de jeu pour un
 *                processus joueur dans le jeu de d'Alembert.
 *
 *                Déroulement d'un tour :
 *                  1. Tirage de deux jetons aléatoires (dx, dy)
 *                  2. Calcul de la case cible
 *                  3. Application des règles spéciales :
 *                       - hors grille → pion immobile
 *                       - case X      → retour forcé en (1,1)
 *                       - case occupée → adversaire renvoyé en (1,1)
 *                  4. Vérification de la condition de victoire
 *                  5. Envoi du coup à l'adversaire via le pipe
 *                  6. Lecture du coup de l'adversaire (attente)
 *
 * Auteurs      : Oumar Leyti Ndiaye , Mouhamed souley Kalilou
 * Cours        : INFO4108 — Programmation Unix
 * Session      : Hiver 2026
 * Sources IA   : Claude Sonnet (Anthropic) — génération du squelette
 * ============================================================
 */

#include "game.h"

/*
 * player_take_turn
 * ----------------
 * Effectue un tour complet pour le joueur identifié par player_id.
 *
 * Paramètres :
 *   gs               — état courant du jeu (modifié en place)
 *   player_id        — identifiant du joueur actif : 0 (J1) ou 1 (J2)
 *   fd_write         — descripteur d'écriture du pipe vers l'adversaire
 *   fd_read_opponent — descripteur de lecture du pipe depuis l'adversaire
 */
void player_take_turn(GameState *gs, int player_id,
                      int fd_write, int fd_read __attribute__((unused)))
{
    /* Identifiant de l'adversaire (0 si je suis 1, et vice-versa) */
    int opponent = 1 - player_id;

    /* ── 1. Tirage des deux jetons ── */
    int dx = token_draw();  /* Déplacement horizontal (abscisse) */
    int dy = token_draw();  /* Déplacement vertical   (ordonnée) */

    printf("[Joueur %d] Tire (%+d, %+d)\n", player_id + 1, dx, dy);

    /* ── 2. Calcul de la case cible ── */
    int new_x = gs->pos[player_id].x + dx;  /* Nouvelle position x */
    int new_y = gs->pos[player_id].y + dy;  /* Nouvelle position y */

    /* Flag : 1 si déplacement naturel, 0 si forcé */
    /* Un déplacement naturel est nécessaire pour gagner en (1,1) */
    int natural = 0;

    /* ── 3. Application des règles spéciales ── */

    if (!grid_in_bounds(new_x, new_y)) {
        /* Règle : case hors grille → le pion ne bouge pas */
        printf("[Joueur %d] Case (%d,%d) hors grille, pion immobile.\n",
               player_id + 1, new_x, new_y);
        new_x = gs->pos[player_id].x;  /* Garder la position actuelle */
        new_y = gs->pos[player_id].y;
        natural = 0;
    }
    else if (grid_is_x_cell(new_x, new_y)) {
        /* Règle : case X → retour forcé en (1,1) sans victoire possible */
        printf("[Joueur %d] Case X ! Retour force en (1,1).\n", player_id + 1);
        new_x = 1;
        new_y = 1;
        natural = 0;  /* Arrivée forcée : pas de victoire possible */
    }
    else {
        /* Règle : case occupée par l'adversaire → adversaire renvoyé en (1,1) */
        if (gs->pos[opponent].x == new_x && gs->pos[opponent].y == new_y) {
            printf("[Joueur %d] Chasse le Joueur %d en (1,1) !\n",
                   player_id + 1, opponent + 1);
            gs->pos[opponent].x = 1;
            gs->pos[opponent].y = 1;
        }
        natural = 1;  /* Déplacement naturel : victoire possible */
    }

    /* Mettre à jour la position du joueur courant */
    gs->pos[player_id].x = new_x;
    gs->pos[player_id].y = new_y;

    /* ── 4. Vérification de la victoire ── */
    /*
     * Un joueur gagne si et seulement si :
     *   - Il arrive en (1,1) PAR UN TIRAGE NATUREL (pas forcé)
     *   - OU il arrive en (11,11) (toujours une victoire)
     */
    int won = natural &&
              ((new_x == WIN_LOW  && new_y == WIN_LOW) ||
               (new_x == WIN_HIGH && new_y == WIN_HIGH));

    if (won) {
        printf("[Joueur %d] *** VICTOIRE ! Position (%d,%d) ***\n",
               player_id + 1, new_x, new_y);
        gs->scores[player_id]++;  /* Incrémenter le score du gagnant */
    }

    /* ── 5. Envoi du coup à l'adversaire via le pipe ── */
    MoveMsg msg = {
        .player_id = player_id,          /* Qui a joué                  */
        .new_x     = new_x,              /* Nouvelle position x         */
        .new_y     = new_y,              /* Nouvelle position y         */
        .game_over = won,                /* 1 si victoire, 0 sinon      */
        .winner    = won ? player_id : -1, /* Gagnant ou -1             */
        .natural   = natural             /* 1 si déplacement naturel    */
    };

    /* Écriture dans le pipe vers l'adversaire */
    write(fd_write, &msg, sizeof(msg));

    /* ── 6. Lecture du coup de l'adversaire ── */
    /* On attend seulement si la partie n'est pas terminée */
    
    //if (!won) {
      //  MoveMsg reply;

        /* Lecture bloquante : on attend que l'adversaire joue */
        //ssize_t n = read(fd_read_opponent, &reply, sizeof(reply));

        //if (n > 0) {
            /* Mettre à jour la position de l'adversaire */
          //  gs->pos[opponent].x = reply.new_x;
           // gs->pos[opponent].y = reply.new_y;

            /* Vérifier si l'adversaire a gagné */
           // if (reply.game_over) {
             //   printf("[Joueur %d] L'adversaire (J%d) a gagne.\n",
               //        player_id + 1, opponent + 1);
               // gs->scores[opponent]++;
           // }
        //}
        /* Si n <= 0, le pipe est fermé : fin de partie */
    //}
}