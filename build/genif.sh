#! /bin/sh

# $Id: genif.sh,v 1.5 2005-06-17 09:39:20 dmitry Exp $
# replacement for genif.pl

infile=$1
shift
srcdir=$1
shift
extra_module_ptrs=$1
shift
awk=$1
shift

if test -z "$infile" || test -z "$srcdir"; then
	echo "please supply infile and srcdir"
	exit 1
fi

module_ptrs=$extra_module_ptrs
header_list=
olddir=`pwd`
cd $srcdir

for ext in ${1+"$@"} ; do
	module_ptrs="	phpext_${ext}_ptr,@NEWLINE@$module_ptrs"
	header_list="$header_list ext/$ext/*.h"
done

includes=`$awk -f ./build/print_include.awk $header_list`

cd $olddir

cat $infile | \
	sed \
	-e "s'@EXT_INCLUDE_CODE@'$includes'" \
	-e "s'@EXT_MODULE_PTRS@'$module_ptrs'" \
	-e 's/@NEWLINE@/\
/g'


