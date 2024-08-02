FROM debian:bookworm

RUN apt-get update \
 && DEBIAN_FRONTEND="noninteractive" apt-get install -y \
      "build-essential" "autoconf" "bison" "re2c" "pkg-config" "zlib1g-dev" "libsqlite3-dev" "libpq-dev" "locales" \
      "valgrind" "llvm" "clang" "git" "gdb" \
 && locale-gen "en_US.UTF-8"

ENV LANGUAGE ""
ENV LANG "en_US.UTF-8"
ENV LC_ALL "C"
ENV PDO_PGSQL_TEST_DSN "pgsql:host=pgsql port=5432 dbname=test user=test password=test"
