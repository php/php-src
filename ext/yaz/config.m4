dnl $Id$

AC_ARG_WITH(yaz,
[  --with-yaz[=DIR]        Include YAZ support (ANSI/NISO Z39.50). DIR is
                          the YAZ bin install directory],
[
	yazconfig=NONE
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_PATH_PROG(yazconfig, yaz-config, NONE)
		else
			if test -r ${withval}/yaz-config; then
				yazconfig=${withval}/yaz-config
			else
				yazconfig=${withval}/bin/yaz-config
			fi
		fi
	fi
	AC_MSG_CHECKING(for YAZ support)
	if test -f $yazconfig; then
  		AC_DEFINE(HAVE_YAZ,1,[Whether you have YAZ])
		. $yazconfig
		for i in $YAZLIB; do
			case $i in
			-l*)
				ii=`echo $i|cut -c 3-`
				AC_ADD_LIBRARY($ii)
				;;
			-L*)
				ii=`echo $i|cut -c 3-`
				AC_ADD_LIBPATH($ii)
				;;
			esac
		done
		for i in $YAZINC; do
			case $i in
			-I*)
				ii=`echo $i|cut -c 3-`
				AC_ADD_INCLUDE($ii)
				;;
			esac
		done
    		AC_MSG_RESULT(yes)
    		PHP_EXTENSION(yaz)
	else
		AC_MSG_RESULT(no)
	fi
],[
	AC_MSG_CHECKING(for YAZ support)
	AC_MSG_RESULT(no)
])
