#! /bin/sh

# $Id: genif.sh,v 1.10 2000-12-20 16:29:07 sas Exp $
# replacement for genif.pl

infile="$1"
shift
srcdir="$1"
shift
extra_module_ptrs="$1"
shift
awk="$1"
shift

if test "$infile" = "" -o "$srcdir" = ""; then
	echo "please supply infile and srcdir"
	exit 1
fi

module_ptrs="$extra_module_ptrs"
header_list=""
olddir=`pwd`
cd $srcdir

for ext in ${1+"$@"} ; do
	module_ptrs="	phpext_${ext}_ptr,\\\n$module_ptrs"
	header_list="$header_list ext/$ext/*.h"
done

includes=`$awk -f $srcdir/build/print_include.awk $header_list`

cd $olddir

cat $infile | \
	sed \
	-e "s'@EXT_INCLUDE_CODE@'$includes'" \
	-e "s'@EXT_MODULE_PTRS@'$module_ptrs'" \
	-e 's/[\]n/\
/g'


