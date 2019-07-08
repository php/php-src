VERSION=5.37
if [[ ! -d libmagic.orig ]]; then
  mkdir libmagic.orig
  wget -O - ftp://ftp.astron.com/pub/file/file-$VERSION.tar.gz \
    | tar -xz --strip-components=2 -C libmagic.orig file-$VERSION/src
fi
diff -u libmagic.orig libmagic | grep -v '^Only in' > libmagic.patch
