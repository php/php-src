#! /bin/sh

# $Id: genif.sh,v 1.1 1999-05-06 20:52:19 sas Exp $
# replacement for genif.pl

infile="$1"
shift
srcdir="$1"
shift

if test "$infile" = "" -o "$srcdir" = ""; then
	echo "please supply infile and srcdir"
	exit 1
fi

cmd1='echo $data | grep @EXT_INCLUDE_CODE@ > /dev/null 2>&1'
cmd2='echo $data | grep @EXT_MODULE_PTRS@ > /dev/null 2>&1'

while read ; do
	data="$REPLY"
	if eval $cmd1 ; then
		for ext in $* ; do
			for pre in php3 php php4 zend; do
				hdrfile="ext/$ext/${pre}_${ext}.h"
				if test -f $hdrfile ; then
					echo "#include \"$hdrfile\""
				fi
			done
		done
	elif eval $cmd2 ; then
		for ext in $* ; do
			echo "	phpext_${ext}_ptr,"
		done
	else
		echo "$data"
	fi
done < $infile
