#! /bin/sh

if test "$2" = ""; then
	echo "usage: $0 basedir depth"
	exit 1
fi

if test "$2" = "0"; then
	exit 0
fi

for i in a b c d e f 0 1 2 3 4 5 6 7 8 9; do
	newpath="$1/$i"
	mkdir $newpath || exit 1
	sh $0 $newpath `expr $2 - 1`
done
