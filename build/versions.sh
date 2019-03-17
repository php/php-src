#!/bin/sh
#
# This script checks if versions of various tools needed to build PHP match the
# required versions.

# Minimum bison version (number = major * 10000 + minor * 100 + patch).
bison_min_version="3.0.2"
bison_min_version_num="30002"

# Non-working bison versions separated by space, e.g. "3.0 3.2".
bison_version_exclude=""

# Minimum required re2c version.
re2c_min_version="1.0.3"
re2c_min_version_num="10003"
re2c_version_exclude=""

# Set some default values.
YACC=${YACC:-bison}
RE2C=${RE2C:-re2c}
SED=${SED:-sed}
get_min_version=0
get_current_version=0
current_version="none"
check="invalid"

# Process command options.
while test $# -gt 0; do
  if test "$1" = "-h" || test "$1" = "--help"; then
    cat << HELP
PHP versions checker

This tool checks if the system's tools versions are supported by PHP.

SYNOPSIS:
  versions.sh [<options>]

OPTIONS:
  -h, --help     Display this help.
  --re2c         Check re2c.
  --bison        Check bison.
  --min-version  Get minimum required version of the selected tool.
  --version      Get current version of the selected tool.

ENVIRONMENT:
  The following optional variables are supported:

  SED             Overrides the path to sed tool.
                  SED=/path/to/sed ./versions.sh
  YACC            Overrides the path to bison.
                  YACC=/path/to/bison ./versions.sh
  RE2C            Overrides the path to re2c.
                  RE2C=/path/to/re2c ./versions.sh
HELP
  exit 0
  fi

  if test "$1" = "--bison"; then
    version_vars=$($YACC --version 2> /dev/null | grep 'GNU Bison' | cut -d ' ' -f 4 | $SED -e 's/\./ /g' | tr -d a-z)
    min_version=$bison_min_version
    min_version_num=$bison_min_version_num
    version_exclude=$bison_version_exclude
  fi

  if test "$1" = "--re2c"; then
    version_vars=$($RE2C --version 2> /dev/null | grep 're2c' | cut -d ' ' -f 2 | sed -e 's/\./ /g')
    current_version_num=$($RE2C --vernum 2> /dev/null)
    min_version=$re2c_min_version
    min_version_num=$re2c_min_version_num
    version_exclude=$re2c_version_exclude
  fi

  if test "$1" = "--min-version"; then
    get_min_version=1
  fi

  if test "$1" = "--version"; then
    get_current_version=1
  fi

  shift
done

if test -n "$version_vars"; then
  set $version_vars
  current_version="${1}.${2}.${3}"

  if test -z "$current_version_num"; then
    current_version_num="`expr ${1} \* 10000 + ${2} \* 100 + ${3}`"
  fi

  if test $current_version_num -ge $min_version_num; then
    check="$current_version (ok)"
    for check_version in $version_exclude; do
      if test "$current_version" = "$check_version"; then
        check="invalid"
        break
      fi
    done
  fi
fi

if test "$get_current_version" = "1"; then
  echo $current_version
elif test "$get_min_version" = "1"; then
  echo $min_version
else
  echo $check
fi
