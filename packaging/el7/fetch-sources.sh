#!/bin/bash
# Download PECL sources listed in sources.sha256 and verify them
set -euo pipefail
cd "$(dirname "$0")/SOURCES"
while read -r sum file url; do
    [ -f "$file" ] || curl -fsSL -o "$file" "$url"
    echo "$sum  $file" | shasum -a 256 -c - >/dev/null
done < ../sources.sha256
