#! /bin/sh

# $Id: genif.sh,v 1.6 1999-05-21 10:05:41 sas Exp $
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

for ext in ${1+"$@"} ; do
	module_ptrs="	phpext_${ext}_ptr,\\\n$module_ptrs"
	for pre in php3_ php_ php4_ zend_ "" ; do
		hdrfile="ext/$ext/${pre}${ext}.h"
		if test -f "$srcdir/$hdrfile" ; then
			includes="#include \"$hdrfile\"\\\n$includes"
		fi
	done
done

cat $infile | \
	sed \
	-e "s'@EXT_INCLUDE_CODE@'$includes'" \
	-e "s'@EXT_MODULE_PTRS@'$module_ptrs'" \
	-e 's/\\n/\
/g'


