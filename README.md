# Jeu de d'Alembert – INFO4108, Hiver 2026

## Auteurs
- Oumar Leyti Ndiaye

## Sources IA
Squelette généré avec l'aide de Claude Sonnet (Anthropic).
Toute la logique métier, la synchronisation des pipes et les tests ont été
réalisés / vérifiés par les auteurs.

---

## Structure des fichiers

| Fichier     | Rôle |
|-------------|------|
| `game.h`    | Constantes, structures (`MoveMsg`, `GameState`, `Point`), prototypes |
| `main.c`    | Processus père : création des pipes, `fork()` ×2, `waitpid()`, scores |
| `grid.c`    | Affichage TUI de la grille 11×11, tests de cases |
| `player.c`  | Logique d'un tour de jeu (tirage, déplacement, règles, pipe) |
| `utils.c`   | Tirage de jeton, lecture/écriture de `scores.txt`, pause clavier |
| `Makefile`  | Compilation avec `gcc -Wall -std=c11` |

---

## Architecture des processus

```
                ┌─────────────────────┐
                │   Processus PÈRE    │
                │  - fork() ×2        │
                │  - waitpid() ×2     │
                │  - score_save()     │
                └──────┬──────┬───────┘
                       │      │
              fork()   │      │   fork()
                       ▼      ▼
          ┌────────────┐      ┌────────────┐
          │  Fils 1    │      │  Fils 2    │
          │ (Joueur 1) │      │ (Joueur 2) │
          │            │◄────►│            │
          └────────────┘pipes └────────────┘
```

### Pipes
- `pipe_1_to_2` : Fils 1 écrit → Fils 2 lit
- `pipe_2_to_1` : Fils 2 écrit → Fils 1 lit

### Message échangé (`MoveMsg`)
```c
typedef struct {
    int player_id;  /* émetteur (0 ou 1)              */
    int new_x;      /* nouvelle position x du pion    */
    int new_y;      /* nouvelle position y du pion    */
    int game_over;  /* 1 si la partie est finie       */
    int winner;     /* id du gagnant (-1 si en cours) */
    int natural;    /* 1 si arrivée naturelle         */
} MoveMsg;
```

---

## Règles implémentées

1. **Grille 11×11** – cases 1-indexées, centre en (6,6).
2. **Tirage** – deux jetons indépendants dans `{-2,-1,0,1,2}`.
3. **Hors grille** – le pion ne bouge pas.
4. **Case X** – le joueur revient en (1,1) *sans* possibilité de victoire.
5. **Case occupée** – l'adversaire est renvoyé en (1,1).
6. **Victoire** – atteindre (1,1) par tirage naturel **ou** (11,11).

### Cases X (selon Figure 1 du sujet)
`(2,4)  (3,9)  (6,8)  (7,5)  (6,2)  (10,2)  (9,10)`
> Vérifier avec l'image du sujet et corriger dans `game.h` si nécessaire.

---

## Compilation & lancement

```bash
make          # compile
make run      # compile + lance
make clean    # nettoyage
```

---

## Travail 

- Conception et rédaction de `game.h` (structures, constantes, prototypes)
- Rédaction de `utils.c` (tirage de jetons, sauvegarde/chargement des scores)
- Rédaction de `grid.c` (affichage de la grille, tests de cases)
- Tests et débogage général
- Rédaction de `player.c` (logique complète d'un tour de jeu)
- Rédaction de `main.c` (fork, pipes, boucle principale, scores)
- Mise en place du Makefile
- Tests et débogage général

## Erreurs détectées et corrections appliquées

**1. Jeu bloqué sur "En attente du coup adverse"**
Les deux processus lisaient le clavier en même temps via `wait_for_key()`.
Correction : suppression de `wait_for_key()` dans le bloc d'attente,
remplacé par `sleep(2)` pour une pause automatique.

**2. Champ `.natural` non reconnu**
Le champ `natural` était utilisé dans `player.c` mais absent
de la structure `MoveMsg` dans `game.h`.
Correction : ajout de `int natural` dans la structure `MoveMsg`.

**3. Erreur `grid_init` non définie**
Le prototype `grid_init()` était déclaré dans `game.h` mais
la fonction n'était jamais écrite dans `grid.c`.
Correction : suppression du prototype inutile dans `game.h`.

**4. Double code dans `main.c`**
Deux versions du code ont été collées l'une après l'autre
par erreur lors de la correction.
Correction : nettoyage complet du fichier `main.c`.

**5. Scores toujours à 0**
Après `fork()`, le père et les fils ont des espaces mémoire
séparés. Le fils mettait à jour ses scores mais le père
ne le savait pas.
Correction : utilisation de `WEXITSTATUS(status)` dans
`run_parent()` pour récupérer l'identifiant du gagnant
via le code de retour du fils.

---

## Ce qui reste à améliorer

- Correction du bug des scores (transmission du gagnant père↔fils)
- Distinguer arrivée naturelle en (1,1) vs arrivée forcée
- Ajouter l'effacement d'écran entre chaque coup
- Interface TUI améliorée (ncurses optionnel)

-
