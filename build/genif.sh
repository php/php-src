#!/bin/sh
#
# Generate internal functions file content based on the provided extensions.
#
# SYNOPSIS:
#   genif.sh <template> <extensions>
#
# ARGUMENTS:
#   template    Path to internal functions template file.
#   extensions  Space delimited list of provided extensions and their locations.
#
# ENVIRONMENT:
#   The following optional variables are supported:
#
#   AWK         Path to the awk program or its command name.
#               AWK=/path/to/awk genif.sh ...
#
# USAGE EXAMPLE:
#   AWK=nawk ./build/genif.sh ./main/internal_functions.c.in "date;ext/date spl;ext/spl" > ./main/internal_functions.c

AWK=${AWK:-awk}
template=$1
shift
extensions="$@"

if test -z "$template"; then
  echo "Please supply template." >&2
  exit 1
fi

header_list=
olddir=$(pwd)

# Go to project root.
cd $(CDPATH= cd -- "$(dirname -- "$0")/../" && pwd -P)

module_ptrs="$(echo $extensions | $AWK -f ./build/order_by_dep.awk)"

for ext in $extensions; do
  ext_dir=$(echo "$ext" | cut -d ';' -f 2)
  header_list="$header_list $ext_dir/*.h*"
done

includes=$($AWK -f ./build/print_include.awk $header_list)

cd $olddir

cat $template | \
  sed \
    -e "s'@EXT_INCLUDE_CODE@'$includes'" \
    -e "s'@EXT_MODULE_PTRS@'$module_ptrs'" \
    -e 's/@NEWLINE@/\
/g'
