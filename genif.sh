#! /bin/sh

# $Id: genif.sh,v 1.7 1999-09-03 17:46:39 sas Exp $
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

olddir=`pwd`
cd $srcdir

for ext in ${1+"$@"} ; do
	module_ptrs="	phpext_${ext}_ptr,\\\n$module_ptrs"
	for header in ext/$ext/*.h ; do
		if grep phpext_ $header >/dev/null 2>&1 ; then
			includes="#include \"$header\"\\\n$includes"
		fi
	done
done

cd $olddir

cat $infile | \
	sed \
	-e "s'@EXT_INCLUDE_CODE@'$includes'" \
	-e "s'@EXT_MODULE_PTRS@'$module_ptrs'" \
	-e 's/\\n/\
/g'


