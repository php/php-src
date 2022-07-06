#!/bin/sh
set -ev

# PostgreSQL tests currently don't work on some architectures.
if test -z "${ARM64}${S390X}"; then
    psql -c "ALTER USER postgres PASSWORD 'postgres';" -U postgres
    psql -c "CREATE DATABASE test;" -U postgres
fi
