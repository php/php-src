# Makefile for php-src/docs
# Copyright © The PHP Group and Contributors.

# If people set these on the make command line, use 'em

SPHINXBUILD ?= sphinx-build
MDFORMAT ?= mdformat

SOURCEDIR = source
BUILDDIR = build

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
MARKDOWN_FILES = $(call rwildcard,$(SOURCEDIR),*.md) $(call rwildcard,$(SOURCEDIR),*.rst)

all : html

.PHONY : check-formatting clean format html
.SUFFIXES : # Disable legacy behavior

check-formatting :
	$(MDFORMAT) --check $(MARKDOWN_FILES)

clean :
	rm -rf -- $(BUILDDIR)

format :
	$(MDFORMAT) $(MARKDOWN_FILES)

html :
	$(SPHINXBUILD) -M $@ $(SOURCEDIR) $(BUILDDIR)
	@printf 'Browse the \e]8;;%s\e\\%s\e]8;;\e\\.\n' \
		"file://$(abspath $(BUILDDIR))/$@/index.$@" "php-src html docs locally"
