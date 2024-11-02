#!/bin/sh

# Command file for Sphinx documentation

if [ -z "$SPHINXBUILD" ]; then
  SPHINXBUILD=sphinx-build
fi
SOURCEDIR=source
BUILDDIR=build

$SPHINXBUILD > /dev/null 2>&1
if [ $? -eq 127 ]; then
  echo
  echo "The 'sphinx-build' command was not found. Make sure you have Sphinx"
  echo "installed, then set the SPHINXBUILD environment variable to point"
  echo "to the full path of the 'sphinx-build' executable. Alternatively you"
  echo "may add the Sphinx directory to PATH."
  echo
  echo "If you don't have Sphinx installed, grab it from"
  echo "https://www.sphinx-doc.org/"
  exit 1
fi

if [ -z "$1" ]; then
  $SPHINXBUILD -M help $SOURCEDIR $BUILDDIR $SPHINXOPTS $O
  exit 0
fi

$SPHINXBUILD -M "$1" $SOURCEDIR $BUILDDIR $SPHINXOPTS $O