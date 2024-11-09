# Makefile for php-src/docs
# Copyright (c) The PHP Group

# If people set these on the make command line, use 'em

SPHINXBUILD ?= sphinx-build

SOURCEDIR = source
BUILDDIR = build
RSTFMT = rstfmt
RSTFMTFLAGS = -w 100

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
FILES = $(call rwildcard,$(SOURCEDIR),*.rst)

all : html

.PHONY : check-formatting clean html preflight
.SUFFIXES : # Disable legacy behavior

check-formatting :
	$(RSTFMT) $(RSTFMTFLAGS) --check $(SOURCEDIR)

clean :
	rm -rf -- $(wildcard $(SOURCEDIR)/.~ $(BUILDDIR))

html : preflight
	$(SPHINXBUILD) -M $@ $(SOURCEDIR) $(BUILDDIR)
	@printf 'Browse the \e]8;;%s\e\\%s\e]8;;\e\\.\n' \
		"file://$(abspath $(BUILDDIR))/$@/index.$@" "php-src html docs locally"

preflight : $(SOURCEDIR)/.~

$(SOURCEDIR)/.~ : $(FILES)
	$(RSTFMT) $(RSTFMTFLAGS) $?
	touch $@
