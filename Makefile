# ============================================================
# Makefile – Jeu de d'Alembert (INFO4108, Hiver 2026)
# ============================================================
# Cibles :
#   make          → compile le projet (binaire : dalembert)
#   make clean    → supprime les fichiers objets et le binaire
#   make run      → compile et lance le jeu
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = dalembert
SRCS    = main.c grid.c player.c utils.c
OBJS    = $(SRCS:.c=.o)

# ── Règle principale ──────────────────────────────────────────
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ── Compilation des fichiers objets ──────────────────────────
%.o: %.c game.h
	$(CC) $(CFLAGS) -c $< -o $@

# ── Raccourcis ───────────────────────────────────────────────
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: run clean
