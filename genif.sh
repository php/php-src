#! /bin/sh

# $Id: genif.sh,v 1.3 1999-05-08 21:44:12 sas Exp $
# replacement for genif.pl

infile="$1"
shift
srcdir="$1"
shift

if test "$infile" = "" -o "$srcdir" = ""; then
	echo "please supply infile and srcdir"
	exit 1
fi

module_ptrs=""
includes=""

# the 'ä' is used as a newline replacement
# its later tr'd to '\n'

for ext in $* ; do
	module_ptrs="	phpext_${ext}_ptr,ä$module_ptrs"
	for pre in php3 php php4 zend; do
		hdrfile="ext/$ext/${pre}_${ext}.h"
		if test -f $hdrfile ; then
			includes="#include \"$hdrfile\"ä$includes"
		fi
	done
done

cat $infile | \
	sed "s'@EXT_INCLUDE_CODE@'$includes'" | \
	sed "s'@EXT_MODULE_PTRS@'$module_ptrs'" | \
	tr ä '\n'

