#!/bin/sh

set -e
cd "$(dirname "$0")"

print_test() {
  echo "Testing: $1 (CC=$CC LD=$LD CFLAGS=$CFLAGS)"
}

exe_def_static_user() {
  print_test "TLS var defined in executable, used in same object"

  rm -f main

  $CC $CFLAGS -ggdb3 -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -o user.o -c user.c
  $CC $CFLAGS -ggdb3 -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -ggdb3 -o main main.c def.o user.o tls.o

  ./main
}

exe_def_shared_user() {
  print_test "TLS var defined in executable, used in shared library"

  rm -f main

  $CC $CFLAGS -ggdb3 -fPIC -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -fPIC -o user.o -c user.c
  $CC $CFLAGS $LDFLAGS -shared -o libuser.so user.o tls.o

  $CC $CFLAGS -ggdb3 -fPIC -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -ggdb3 -fPIC -o main main.c def.o -Wl,-rpath,$(pwd) -L. -luser

  ./main
}

shared_def_static_user() {
  print_test "TLS var defined in shared library, used in same object"

  rm -f main

  $CC $CFLAGS -ggdb3 -fPIC -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -fPIC -o user.o -c user.c
  $CC $CFLAGS -ggdb3 -fPIC -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -shared -o libdef.so def.o user.o tls.o

  $CC $CFLAGS $LDFLAGS -ggdb3 -fPIC -o main main.c -Wl,-rpath,$(pwd) -L. -ldef

  ./main
}

shared_def_shared_user() {
  print_test "TLS var defined in shared object, used in other shared object"

  rm -f main

  $CC $CFLAGS -ggdb3 -fPIC -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -fPIC -o user.o -c user.c
  $CC $CFLAGS $LDFLAGS -shared -o libuser.so user.o tls.o

  $CC $CFLAGS -ggdb3 -fPIC -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -shared -o libdef.so def.o -Wl,-rpath,$(pwd) -L. -luser

  $CC $CFLAGS $LDFLAGS -ggdb3 -fPIC -o main main.c -Wl,-rpath,$(pwd) -L. -ldef

  ./main
}

shared_def_static_user_no_surplus() {
  print_test "TLS var defined in shared library, used in same object. Likely no static TLS surplus."

  rm -f main

  $CC $CFLAGS -ggdb3 -fPIC -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -fPIC -o user.o -c user.c
  $CC $CFLAGS -DNO_SURPLUS -ggdb3 -fPIC -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -shared -o libdef.so def.o tls.o user.o

  $CC $CFLAGS -DNO_SURPLUS $LDFLAGS -ggdb3 -fPIC -o main main.c -Wl,-rpath,$(pwd) -L. -ldef

  ./main
}

shared_def_shared_user_no_surplus() {
  print_test "TLS var defined in shared object, used in other shared object. Likely no static TLS surplus."

  rm -f main

  $CC $CFLAGS -ggdb3 -fPIC -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -fPIC -o user.o -c user.c
  $CC $CFLAGS $LDFLAGS -shared -o libuser.so user.o tls.o

  $CC $CFLAGS -DNO_SURPLUS -ggdb3 -fPIC -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -shared -o libdef.so def.o -Wl,-rpath,$(pwd) -L. -luser

  $CC $CFLAGS -DNO_SURPLUS $LDFLAGS -ggdb3 -fPIC -o main main.c -Wl,-rpath,$(pwd) -L. -ldef

  ./main
}

dl_def_static_user() {
  print_test "TLS var defined in dl()'ed object, used in same object"

  rm -f main

  $CC $CFLAGS -ggdb3 -fPIC -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -fPIC -o user.o -c user.c
  $CC $CFLAGS -ggdb3 -fPIC -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -shared -o libdef.so def.o user.o tls.o

  $CC $CFLAGS $LDFLAGS -DDL_DECL -ggdb3 -fPIC -o main main.c

  ./main
}

dl_def_shared_user() {
  print_test "TLS var defined in dl()'ed object, used in other shared object"

  rm -f main

  $CC $CFLAGS -ggdb3 -fPIC -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -fPIC -o user.o -c user.c
  $CC $CFLAGS $LDFLAGS -shared -o libuser.so user.o tls.o

  $CC $CFLAGS -ggdb3 -fPIC -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -shared -o libdef.so def.o -Wl,-rpath,$(pwd) -L. -luser

  $CC $CFLAGS $LDFLAGS -DDL_DECL -ggdb3 -fPIC -o main main.c

  ./main
}

dl_def_static_user_no_surplus() {
  print_test "TLS var defined in dl()'ed object, used in same object. Likely no surplus TLS"

  rm -f main

  $CC $CFLAGS -ggdb3 -fPIC -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -fPIC -o user.o -c user.c
  $CC $CFLAGS -DNO_SURPLUS -ggdb3 -fPIC -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -shared -o libdef.so def.o user.o tls.o

  $CC $CFLAGS -DNO_SURPLUS $LDFLAGS -DDL_DECL -ggdb3 -fPIC -o main main.c

  ./main
}

dl_def_shared_user_no_surplus() {
  print_test "TLS var defined in dl()'ed object, used in other shared object. Likely no surplus TLS"

  rm -f main

  $CC $CFLAGS -ggdb3 -fPIC -o tls.o -c $TLSC
  $CC $CFLAGS -ggdb3 -fPIC -o user.o -c user.c
  $CC $CFLAGS $LDFLAGS -shared -o libuser.so user.o tls.o

  $CC $CFLAGS -DNO_SURPLUS -ggdb3 -fPIC -o def.o -c def.c
  $CC $CFLAGS $LDFLAGS -shared -o libdef.so def.o -Wl,-rpath,$(pwd) -L. -luser

  $CC $CFLAGS -DNO_SURPLUS $LDFLAGS -DDL_DECL -ggdb3 -fPIC -o main main.c

  ./main
}

if [ -z "$TLSC" ]; then
  echo "Variable TLSC is not set" >&2
  exit 1
fi

root=$(pwd)/../../../../..

# Cheap musl detection
if test -f /etc/alpine-release; then
  MUSL="$CFLAGS -D__MUSL__"
else
  MUSL=
fi

if [ "${STATIC_SUPPORT:-yes}" = "yes" ]; then
  STATIC=-static
fi

for CC in clang gcc; do
  if [ $CC = gcc ] && [ -f /etc/freebsd-update.conf ]; then
    RPATH=-Wl,-rpath,/usr/local/lib/gcc13
  else
    RPATH=
  fi
  case $CC in
    gcc)
      LDs=""
      for l in bdf gold; do
        if command -v ld.$l >/dev/null 2>&1; then
        LDs="$LDs $l"
        fi
      done
      if [ -z "$LDs" ]; then
        LDs=ld
      fi
      ;;
    clang)
      LDs="ld"
      if command -v ld.lld >/dev/null 2>&1; then
        LDs="$LDs lld"
      fi
      ;;
  esac
  for LD in $LDs; do
    for opt in -O0 -O3; do
      CFLAGS="$MACHINE $MUSL $opt -Werror -I$root/ext/opcache -I$root/Zend -I$root"
      LDFLAGS="$MACHINE -fuse-ld=$LD $RPATH"

      for pic in "-fPIC" "-fno-PIC $STATIC"; do
        CFLAGS="$CFLAGS $pic" exe_def_static_user
      done
      shared_def_static_user
      shared_def_static_user_no_surplus
      dl_def_static_user
      dl_def_static_user_no_surplus
      if [ "$EXTERN_TLS_SUPPORT" = yes ]; then
        exe_def_shared_user
        shared_def_shared_user
        shared_def_shared_user_no_surplus
        dl_def_shared_user
        dl_def_shared_user_no_surplus
      fi
    done
  done
done

echo "All OK" >&2
