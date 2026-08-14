# Makefile for php-src/docs
# Copyright © The PHP Group and Contributors.

# If people set these on the make command line, use 'em

SPHINXBUILD ?= sphinx-build

SOURCEDIR = source
BUILDDIR = build

all : html

.PHONY : clean html
.SUFFIXES : # Disable legacy behavior

clean :
	rm -rf -- $(BUILDDIR)

html :
	$(SPHINXBUILD) -M $@ $(SOURCEDIR) $(BUILDDIR)
	@printf 'Browse the \e]8;;%s\e\\%s\e]8;;\e\\.\n' \
		"file://$(abspath $(BUILDDIR))/$@/index.$@" "php-src html docs locally"
