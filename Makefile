INSTALLDIR=$(XDG_HOME_BIN)
SRC_DIR=src
CFLAGS=-lm -std=c11 -O2 -Werror -pedantic-errors -Wall -D_DEFAULT_SOURCE
SRC := $(shell find $(SRC_DIR) -name "*.c")

all:
	$(CC) $(CFLAGS) $(SRC) -o $(INSTALLDIR)/aux_lwallpaper

