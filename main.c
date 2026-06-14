/*
 * ============================================================
 * Fichier      : main.c
 * Description  : Point d'entrée du programme.
 *                - Charge les scores existants
 *                - Crée deux pipes bidirectionnels
 *                - Fork deux processus fils (un par joueur)
 *                - Chaque fils joue en boucle
 *                - Le père attend la fin et sauvegarde les scores
 *                - Demande si on veut rejouer à la fin
 *
 * Architecture des pipes :
 *   pipe_1_to_2 : Fils 1 écrit → Fils 2 lit
 *   pipe_2_to_1 : Fils 2 écrit → Fils 1 lit
 *
 * Auteurs      : Oumar Leyti Ndiaye , Mouhamed souley Kalilou
 * Cours        : INFO4108 — Programmation Unix
 * Session      : Hiver 2026
 * Sources IA   : Claude Sonnet (Anthropic) — génération du squelette
 * ============================================================
 */

#include "game.h"

/* ── Prototypes des fonctions locales ── */
static void run_player(int player_id, int fd_write, int fd_read,
                       const int scores_init[NUM_PLAYERS]);
static void run_parent(pid_t pid1, pid_t pid2, int scores[NUM_PLAYERS]);
static int  ask_replay(void);


int main(void)
{
    /* Initialiser le générateur aléatoire */
    srand((unsigned)time(NULL));

    /* Charger les scores depuis le fichier */
    int scores[NUM_PLAYERS];
    score_load(scores);

    /* Boucle principale — une itération = une partie */
    while (1) {

        /* Création des deux pipes */
        int pipe_1_to_2[2];
        int pipe_2_to_1[2];

        if (pipe(pipe_1_to_2) < 0 || pipe(pipe_2_to_1) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        /* Fork du processus fils 1 (Joueur 1) */
        pid_t pid1 = fork();
        if (pid1 < 0) { perror("fork pid1"); exit(EXIT_FAILURE); }

        if (pid1 == 0) {
            /* On est dans le FILS 1 */
            close(pipe_1_to_2[0]); /* Fils1 ne lit pas son propre pipe */
            close(pipe_2_to_1[1]); /* Fils1 n'écrit pas dans p21       */

            run_player(0, pipe_1_to_2[1], pipe_2_to_1[0], scores);

            close(pipe_1_to_2[1]);
            close(pipe_2_to_1[0]);
            exit(EXIT_SUCCESS);
        }

        /* Fork du processus fils 2 (Joueur 2) */
        pid_t pid2 = fork();
        if (pid2 < 0) { perror("fork pid2"); exit(EXIT_FAILURE); }

        if (pid2 == 0) {
            /* On est dans le FILS 2 */
            close(pipe_2_to_1[0]); /* Fils2 ne lit pas son propre pipe */
            close(pipe_1_to_2[1]); /* Fils2 n'écrit pas dans p12       */

            run_player(1, pipe_2_to_1[1], pipe_1_to_2[0], scores);

            close(pipe_2_to_1[1]);
            close(pipe_1_to_2[0]);
            exit(EXIT_SUCCESS);
        }

        /* On est dans le PÈRE */
        /* Le père ferme toutes les extrémités */
        close(pipe_1_to_2[0]); close(pipe_1_to_2[1]);
        close(pipe_2_to_1[0]); close(pipe_2_to_1[1]);

        /* Attendre les fils et sauvegarder les scores */
        run_parent(pid1, pid2, scores);

        /* Demander si on veut rejouer */
        if (!ask_replay()) break;
    }

    printf("\nAu revoir ! Scores finaux : J1=%d  J2=%d\n",
           scores[0], scores[1]);
    return 0;
}

/* 
 * run_player
 * ----------
 * Boucle de jeu d'un processus fils. */
static void run_player(int player_id, int fd_write, int fd_read,
                       const int scores_init[NUM_PLAYERS])
{
    /* Initialiser l'état du jeu */
    GameState gs;
    gs.pos[0].x = gs.pos[0].y = CENTER;
    gs.pos[1].x = gs.pos[1].y = CENTER;
    gs.scores[0]      = scores_init[0];
    gs.scores[1]      = scores_init[1];
    gs.current_player = 0; /* Joueur 1 commence toujours */

    printf("[Joueur %d] Demarre (PID=%d)\n", player_id + 1, getpid());

    while (1) {

    if (gs.current_player == player_id) {
        /* C'est notre tour de jouer */
        grid_display(&gs);
        //wait_for_key();
        sleep(2) ;

        player_take_turn(&gs, player_id, fd_write, fd_read);

        /* Vérifier si on a gagné */
       /* Vérifier si on a gagné */
        int px = gs.pos[player_id].x;
        int py = gs.pos[player_id].y;
        if ((px == WIN_LOW  && py == WIN_LOW) ||
            (px == WIN_HIGH && py == WIN_HIGH)) {
            grid_display(&gs);
            printf("[Joueur %d] *** VICTOIRE ! ***\n", player_id + 1);
            exit(player_id + 1);
        }

        /* Vérifier si l'adversaire a gagné */
        int ox = gs.pos[1 - player_id].x;
        int oy = gs.pos[1 - player_id].y;
        if ((ox == WIN_LOW  && oy == WIN_LOW) ||
            (ox == WIN_HIGH && oy == WIN_HIGH)) {
            grid_display(&gs);
            exit(EXIT_SUCCESS);
        }

        /* Passer le tour à l'adversaire */
        gs.current_player = 1 - player_id;

    } else {
        /* L'adversaire joue — on attend son message via le pipe */
        /* Pas de wait_for_key ici : c'est le pipe qui bloque naturellement */

        MoveMsg msg;
        ssize_t n = read(fd_read, &msg, sizeof(msg));

        /* Si le pipe est fermé, la partie est terminée */
        if (n <= 0) break;

        /* Mettre à jour la position de l'adversaire */
        gs.pos[1 - player_id].x = msg.new_x;
        gs.pos[1 - player_id].y = msg.new_y;

        /* Vérifier si l'adversaire a gagné */
        if (msg.game_over) {
            printf("[Joueur %d] L'adversaire a gagne !\n", player_id + 1);
            break;
        }

        /* C'est maintenant notre tour */
        gs.current_player = player_id;
    }
}
}
/* 
  run_parent

 * Le père attend les fils et sauvegarde les scores.  */

static void run_parent(pid_t pid1, pid_t pid2, int scores[NUM_PLAYERS])
{
    int status1, status2;

    /* Attendre les deux fils */
    waitpid(pid1, &status1, 0);
    printf("[Pere] Fils 1 (PID=%d) termine.\n", pid1);

    waitpid(pid2, &status2, 0);
    printf("[Pere] Fils 2 (PID=%d) termine.\n", pid2);

    /* Récupérer le gagnant via le code de retour du fils */
    if (WIFEXITED(status1)) {
        int winner = WEXITSTATUS(status1);
        if (winner == 1 || winner == 2) {
            scores[winner - 1]++;
            printf("[Pere] Gagnant : Joueur %d\n", winner);
        }
    }
    if (WIFEXITED(status2)) {
        int winner = WEXITSTATUS(status2);
        if (winner == 1 || winner == 2) {
            scores[winner - 1]++;
            printf("[Pere] Gagnant : Joueur %d\n", winner);
        }
    }

    /* Sauvegarder les scores */
    score_save(scores);
}

/* 
 * ask_replay
 * ----------
 * Demande si l'utilisateur veut rejouer.*/
static int ask_replay(void)
{
    printf("\nVoulez-vous rejouer ? (o/n) : ");
    fflush(stdout);

    char buf[8];
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;

    return (buf[0] == 'o' || buf[0] == 'O');
}