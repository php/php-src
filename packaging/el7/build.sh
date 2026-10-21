#!/bin/bash
# Build php70 + php70-pecl RPMs for CentOS 7 in Docker.
#   ./build.sh [x86_64|aarch64]      (default: x86_64)
# Output: packaging/el7/out/<arch>/*.rpm
set -euo pipefail

ARCH=${1:-x86_64}
case "$ARCH" in
  x86_64)  PLATFORM=linux/amd64 ;;
  aarch64) PLATFORM=linux/arm64 ;;
  *) echo "unknown arch $ARCH" >&2; exit 1 ;;
esac

HERE=$(cd "$(dirname "$0")" && pwd)
TOP=$(git -C "$HERE" rev-parse --show-toplevel)
IMAGE=php70-el7-build:$ARCH
OUT=$HERE/out/$ARCH

"$HERE/fetch-sources.sh"

# php source from the committed tree (uncommitted changes are NOT included)
git -C "$TOP" archive --format=tar.gz --prefix=php-7.0.33-sec/ \
    -o "$HERE/SOURCES/php-7.0.33-sec.tar.gz" HEAD -- . ':!packaging'

docker build --platform "$PLATFORM" -t "$IMAGE" "$HERE"

mkdir -p "$OUT"
docker run --rm --platform "$PLATFORM" \
    -v "$HERE":/pkg:ro -v "$OUT":/out \
    "$IMAGE" bash -euo pipefail -c '
mkdir -p ~/rpmbuild/{SOURCES,SPECS}
cp /pkg/SOURCES/* ~/rpmbuild/SOURCES/
cp /pkg/*.spec ~/rpmbuild/SPECS/
rpmbuild -ba ~/rpmbuild/SPECS/php70.spec
yum -y localinstall ~/rpmbuild/RPMS/*/php70-7.0.33-*.rpm ~/rpmbuild/RPMS/*/php70-devel-*.rpm
rpmbuild -ba ~/rpmbuild/SPECS/php70-pecl.spec
cp ~/rpmbuild/RPMS/*/*.rpm ~/rpmbuild/SRPMS/*.rpm /out/
'
ls -la "$OUT"
