#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 /somewhere/.htaccess"
	exit 1
fi

if [ ! -w $1 ]; then
	echo "You cannot write to $1"
	exit 1
fi

TMPFILE=tmpfile.$$

awk -f htaccessfix.awk <$1 >$TMPFILE

if [ "$?" != 0 ]; then
	exit 1
fi

mv -f $1 $1.orig
mv -f $TMPFILE $1
exit 0

