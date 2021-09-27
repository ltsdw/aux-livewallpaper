INSTALLDIR=$(XDG_BIN_HOME)
BUILDDIR=build
SRC_DIR=src
CFLAGS=-lm -std=c11 -O2 -Werror -Wall -D_DEFAULT_SOURCE
SRC := $(shell find $(SRC_DIR) -name "*.c")
TARGET=aux_lwallpaper

all:
	@echo make build
	@echo make install
	@echo make clean

build:
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(SRC) -o $(BUILDDIR)/$(TARGET)

install:
	install -Dm750 $(BUILDDIR)/$(TARGET) $(INSTALLDIR)

clean:
	rm -rf $(BUILDDIR)

