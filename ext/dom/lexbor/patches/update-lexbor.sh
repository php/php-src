#!/usr/bin/env bash

set -e

PATCHES_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LEXBOR_REPO="https://github.com/lexbor/lexbor"
LEXBOR_REF="v2.7.0"
LEXBOR_BASE="$(cd "$PATCHES_DIR/.." && pwd)"
LEXBOR_DIR="$LEXBOR_BASE/lexbor"
LEXBOR_TMP_DIR="$LEXBOR_BASE/lexbor_tmp"
LEXBOR_COMPONENTS=(core css dom encoding html ns ports selectors tag)
LEXBOR_PRESERVED_FILES=(
    selectors-adapted/selectors.c
    selectors-adapted/selectors.h
)

# Clone
git clone "$LEXBOR_REPO" "$LEXBOR_TMP_DIR"
(cd "$LEXBOR_TMP_DIR" && git checkout "$LEXBOR_REF")

# Apply patches
mapfile -t patches < <(ls "$PATCHES_DIR"/*.patch)
cd "$LEXBOR_TMP_DIR"
for patch in "${patches[@]}"; do
    if ! git am -3 "$patch"; then
        read -p "Patch $(basename "$patch") did not apply cleanly. Resolve, stage and press Enter to continue. "
        git am --continue
    fi
done

# Refresh patches
NUM_PATCHES=${#patches[@]}
git format-patch "HEAD~$NUM_PATCHES" -o "$PATCHES_DIR"

# Run code-generation tools
(cd "$LEXBOR_TMP_DIR/utils/lexbor/encoding" && python3 single-byte.py)
(cd "$LEXBOR_TMP_DIR/utils/lexbor/html" && python3 tokenizer_entities_bst.py)

# (Re)move files
mv source/lexbor/encoding/multi_res.c  source/lexbor/encoding/multi.c
mv source/lexbor/encoding/range_res.c  source/lexbor/encoding/range.c
mv source/lexbor/encoding/single_res.c source/lexbor/encoding/single.c
rm source/lexbor/html/serialize.c
rm source/lexbor/ports/posix/lexbor/core/fs.c
rm source/lexbor/ports/posix/lexbor/core/perf.c
rm source/lexbor/ports/windows_nt/lexbor/core/fs.c
rm source/lexbor/ports/windows_nt/lexbor/core/perf.c
find . -name "*.cmake" -delete

# Copy preserved files
for f in "${LEXBOR_PRESERVED_FILES[@]}"; do
    dst="source/lexbor/$f"
    mkdir -p "$(dirname "$dst")"
    cp "$LEXBOR_DIR/$f" "$dst"
done

# Replace components
for dir in "${LEXBOR_COMPONENTS[@]}"; do
    dst="$LEXBOR_DIR/$dir"
    rm -rf "$dst"
    cp -r "source/lexbor/$dir" "$dst"
done
rm -rf "$LEXBOR_TMP_DIR"
