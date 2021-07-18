INSTALLDIR=$(XDG_BIN_HOME)
BUILDDIR=build
SRC_DIR=src
CFLAGS=-lm -std=c11 -O2 -Werror -pedantic-errors -Wall -D_DEFAULT_SOURCE
SRC := $(shell find $(SRC_DIR) -name "*.c")
TARGET=aux_lwallpaper

build:
	$(CC) $(CFLAGS) $(SRC) -o $(BUILDDIR)/$(TARGET)

install:
	install -Dm755 $(BUILDDIR)/$(TARGET) $(INSTALLDIR)

all:
	echo build/install
