#! /bin/sh

## (From INN-1.4, written by Rich Salz)
##  $Revision: 1.1 $
##  A script to install files and directories.

PROGNAME=`basename $0`

##  Paths to programs.  CHOWN and WHOAMI are checked below.
CHOWN=chown
CHGRP=chgrp
CHMOD=chmod
CP=cp
LN=ln
MKDIR=mkdir
MV=mv
RM=rm
STRIP=strip
WHOAMI=whoami

##  Some systems don't support -x, so we have to use -f.
if [ ${CHOWN} = chown ] ; then
    if [ -f /etc/chown ] ; then
	CHOWN=/etc/chown
    else
	if [ -f /usr/etc/chown ] ; then
	    CHOWN=/usr/etc/chown
	fi
    fi
fi

if [ ${WHOAMI} = whoami ] ; then
    if [ -f /usr/ucb/whoami ] ; then
	WHOAMI=/usr/ucb/whoami
    fi
fi

##  Defaults.
CHOWNIT=false
CHGROUPIT=false
CHMODIT=false
STRIPIT=false
BACKIT=false
TOUCHIT=true
SAVESRC=false
ROOT=unknown

##  Process JCL.
MORETODO=true
while ${MORETODO} ; do
    case X"$1" in
    X-b)
	BACKIT=true
	BACKUP="$2"
	shift
	;;
    X-b*)
	BACKIT=true
	BACKUP=`expr "$1" : '-b\(.*\)'`
	;;
    X-c)
	SAVESRC=true
	;;
    X-g)
	GROUP="$2"
	CHGROUPIT=true
	shift
	;;
    X-g*)
	GROUP=`expr "$1" : '-g\(.*\)'`
	CHGROUPIT=true
	;;
    X-G)
	case ${ROOT} in
	unknown)
	    case `${WHOAMI}` in
	    root)
		ROOT=true
		;;
	    *)
		ROOT=false
		;;
	    esac
	    ;;
	esac
	GROUP="$2"
	shift
	${ROOT} && CHGROUPIT=true
	;;
    X-G*)
	case ${ROOT} in
	unknown)
	    case `${WHOAMI}` in
	    root)
		ROOT=true
		;;
	    *)
		ROOT=false
		;;
	    esac
	    ;;
	esac
	if ${ROOT} ; then
	    GROUP=`expr "$1" : '-g\(.*\)'`
	    CHGROUPIT=true
	fi
	;;
    X-m)
	MODE="$2"
	CHMODIT=true
	shift
	;;
    X-m*)
	MODE=`expr "$1" : '-m\(.*\)'`
	CHMODIT=true
	;;
    X-n)
	TOUCHIT=false
	;;
    X-o)
	OWNER="$2"
	CHOWNIT=true
	shift
	;;
    X-o*)
	OWNER=`expr "$1" : '-o\(.*\)'`
	CHOWNIT=true
	;;
    X-O)
	case ${ROOT} in
	unknown)
	    case `${WHOAMI}` in
	    root)
		ROOT=true
		;;
	    *)
		ROOT=false
		;;
	    esac
	    ;;
	esac
	OWNER="$2"
	shift
	${ROOT} && CHOWNIT=true
	;;
    X-O*)
	case ${ROOT} in
	unknown)
	    case `${WHOAMI}` in
	    root)
		ROOT=true
		;;
	    *)
		ROOT=false
		;;
	    esac
	    ;;
	esac
	if ${ROOT} ; then
	    OWNER=`expr "$1" : '-o\(.*\)'`
	    CHOWNIT=true
	fi
	;;
    X-s)
	STRIPIT=true
	;;
    X--)
	shift
	MORETODO=false
	;;
    X-*)
	echo "${PROGNAME}:  Unknown flag $1" 1>&2
	exit 1
	;;
    *)
	MORETODO=false
	;;
    esac
    ${MORETODO} && shift
done

##  Process arguments.
if [ $# -ne 2 ] ; then
    echo "Usage:  ${PROGNAME} [flags] source destination"
    exit 1
fi

##  Making a directory?
if [ X"$1" = X. ] ; then
    DEST="$2"
    if [ ! -d "${DEST}" ] ; then
	${MKDIR} "${DEST}" || exit 1
    fi
    if ${CHOWNIT} ; then
	${CHOWN} "${OWNER}" "${DEST}" || exit 1
    fi
    if ${CHGROUPIT} ; then
	${CHGRP} "${GROUP}" "${DEST}" || exit 1
    fi
    if ${CHMODIT} ; then
	umask 0
	${CHMOD} "${MODE}"  "${DEST}" || exit 1
    fi
    exit 0
fi

##  Get the destination and a temp file in the destination diretory.
if [ -d "$2" ] ; then
    DEST="$2/$1"
    TEMP="$2/$$.tmp"
else
    DEST="$2"
    TEMP="`expr "$2" : '\(.*\)/.*'`/$$.tmp"
fi

##  If not given the same name, we must try to copy.
if [ X"$1" != X"$2" -o $SAVESRC ] ; then
    if cmp -s "$1" "${DEST}" ; then
	##  Files are same; touch or not.
	${TOUCHIT} && touch "${DEST}"
    else
	##  If destination exists and we wish to backup, link to backup.
	if [ -f "${DEST}" ] ; then
	    if ${BACKIT} ; then
		${RM} -f "${DEST}${BACKUP}"
		${LN} "${DEST}" "${DEST}${BACKUP}"
	    fi
	fi
	##  Copy source to the right dir, then move to right spot.
	##  Done in two parts so we can hope for atomicity.
	${RM} -f "${TEMP}" || exit 1
	${CP} "$1" "${TEMP}" || exit 1
	${MV} -f "${TEMP}" "${DEST}" || exit 1
    fi
fi

##  Strip and set the modes.
if ${STRIPIT} ; then
    ${STRIP} "${DEST}" || exit 1
fi
if ${CHOWNIT} ; then
    ${CHOWN} "${OWNER}" "${DEST}" || exit 1
fi
if ${CHGROUPIT} ; then
    ${CHGRP} "${GROUP}" "${DEST}" || exit 1
fi
if ${CHMODIT} ; then
    umask 0
    ${CHMOD} "${MODE}"  "${DEST}" || exit 1
fi
exit 0
