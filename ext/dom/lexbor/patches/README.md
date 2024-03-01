# Lexbor patches

Upon syncing the Lexbor sources, the patches in this directory need to be applied.
The current Lexbor version is 2.4.0.

## Overview

This contains the following patch files in mailbox format.

* 0001-Expose-line-and-column-information-for-use-in-PHP.patch
  A PHP specific patch to expose the line and column number to PHP.
* 0001-Track-implied-added-nodes-for-options-use-in-PHP.patch
  A PHP specific patch to track implied added nodes for options.
* 0001-Patch-out-CSS-parser.patch
  A PHP specific patch to patch out the CSS parser, which is unused and hence this patch reduces the binary size.
* 0001-Patch-utilities-and-data-structure-to-be-able-to-gen.patch
  A PHP specific patch to patch utilities and data structure to be able to generate smaller lookup tables.
  This patch won't be upstreamed because it breaks generality of those data structures, i.e. it only works
  because we only use it for character encoding.
* 0001-Remove-unused-upper-case-tag-static-data.patch
  A PHP specific patch to remove unused upper case tag static data. This shrinks the static data size.
* 0001-Shrink-size-of-static-binary-search-tree.patch
  A PHP specific patch to shrink the size of the static binary search tree for entities.
  This shrinks the static data size and reduces data cache pressure.

  **Note** for this patch the utilities to generate the tables are also patched.
  Make sure to apply on a fresh Lexbor clone and run (in `lexbor/utils/encoding`): `python3 single-byte.py` and `python3 multi-byte.py` to generate the tables.
  Also run `python3 tokenizer_entities_bst.py` to generate the static binary search tree for entities.

## How to apply

* cd into `ext/dom/lexbor/lexbor`
* `git am -3 ../patches/0001-Expose-line-and-column-information-for-use-in-PHP.patch`
* `git am -3 ../patches/0001-Track-implied-added-nodes-for-options-use-in-PHP.patch`
* `git am -3 ../patches/0001-Patch-out-CSS-parser.patch`
* `git am -3 ../patches/0001-Patch-utilities-and-data-structure-to-be-able-to-gen.patch`
* `git am -3 ../patches/0001-Remove-unused-upper-case-tag-static-data.patch`
* `git am -3 ../patches/0001-Shrink-size-of-static-binary-search-tree.patch`
* `git reset HEAD~6` # 6 is the number of commits created by the above commands
