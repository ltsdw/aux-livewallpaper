INSTALLDIR=$(XDG_BIN_HOME)
SRC_DIR=src
CFLAGS=-lm -std=c11 -O2 -Werror -pedantic-errors -Wall -D_DEFAULT_SOURCE
SRC := $(shell find $(SRC_DIR) -name "*.c")
TARGET=aux_lwallpaper

all:
	$(CC) $(CFLAGS) $(SRC) -o $(INSTALLDIR)/$(TARGET)

