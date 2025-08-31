#!/bin/sh
##
##  GNU shtool -- The GNU Portable Shell Tool
##  Copyright (c) 1994-2008 Ralf S. Engelschall <rse@engelschall.com>
##
##  See http://www.gnu.org/software/shtool/ for more information.
##  See ftp://ftp.gnu.org/gnu/shtool/ for latest version.
##
##  Version:  2.0.8 (18-Jul-2008)
##  Contents: 5/19 available modules
##

##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
##  General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
##  USA, or contact Ralf S. Engelschall <rse@engelschall.com>.
##
##  NOTICE: Given that you include this file verbatim into your own
##  source tree, you are justified in saying that it remains separate
##  from your package, and that this way you are simply just using GNU
##  shtool. So, in this situation, there is no requirement that your
##  package itself is licensed under the GNU General Public License in
##  order to take advantage of GNU shtool.
##

##
##  Usage: shtool [<options>] [<cmd-name> [<cmd-options>] [<cmd-args>]]
##
##  Available commands:
##    echo       Print string with optional construct expansion
##    install    Install a program, script or datafile
##    mkdir      Make one or more directories
##    platform   Platform Identification Utility
##    path       Deal with program paths
##
##  Not available commands (because module was not built-in):
##    mdate      Pretty-print modification time of a file or dir
##    table      Pretty-print a field-separated list as a table
##    prop       Display progress with a running propeller
##    move       Move files with simultaneous substitution
##    mkln       Make link with calculation of relative paths
##    mkshadow   Make a shadow tree through symbolic links
##    fixperm    Fix file permissions inside a source tree
##    rotate     Logfile rotation
##    tarball    Roll distribution tarballs
##    subst      Apply sed(1) substitution operations
##    arx        Extended archive command
##    slo        Separate linker options by library class
##    scpp       Sharing C Pre-Processor
##    version    Maintain a version information file
##

#   maximum Bourne-Shell compatibility
if [ ".$ZSH_VERSION" != . ] && (emulate sh) >/dev/null 2>&1; then
    #   reconfigure zsh(1)
    emulate sh
    NULLCMD=:
    alias -g '${1+"$@"}'='"$@"'
elif [ ".$BASH_VERSION" != . ] && (set -o posix) >/dev/null 2>&1; then
    #   reconfigure bash(1)
    set -o posix
fi

#   maximum independence of NLS nuisances
for var in \
    LANG LANGUAGE LC_ADDRESS LC_ALL LC_COLLATE LC_CTYPE LC_IDENTIFICATION \
    LC_MEASUREMENT LC_MESSAGES LC_MONETARY LC_NAME LC_NUMERIC LC_PAPER \
    LC_TELEPHONE LC_TIME
do
    if (set +x; test -z "`(eval $var=C; export $var) 2>&1`"); then
        eval $var=C; export $var
    else
        unset $var
    fi
done

#   initial command line handling
if [ $# -eq 0 ]; then
    echo "$0:Error: invalid command line" 1>&2
    echo "$0:Hint:  run \`$0 -h' for usage" 1>&2
    exit 1
fi
if [ ".$1" = ".-h" ] || [ ".$1" = ".--help" ]; then
    echo "This is GNU shtool, version 2.0.8 (18-Jul-2008)"
    echo 'Copyright (c) 1994-2008 Ralf S. Engelschall <rse@engelschall.com>'
    echo 'Report bugs to <bug-shtool@gnu.org>'
    echo ''
    echo 'Usage: shtool [<options>] [<cmd-name> [<cmd-options>] [<cmd-args>]]'
    echo ''
    echo 'Available global <options>:'
    echo '  -v, --version   display shtool version information'
    echo '  -h, --help      display shtool usage help page (this one)'
    echo '  -d, --debug     display shell trace information'
    echo '  -r, --recreate  recreate this shtool script via shtoolize'
    echo ''
    echo 'Available <cmd-name> [<cmd-options>] [<cmd-args>]:'
    echo '  echo     [-n|--newline] [-e|--expand] [<string> ...]'
    echo '  install  [-v|--verbose] [-t|--trace] [-d|--mkdir] [-c|--copy]'
    echo '           [-C|--compare-copy] [-s|--strip] [-m|--mode <mode>]'
    echo '           [-o|--owner <owner>] [-g|--group <group>] [-e|--exec'
    echo '           <sed-cmd>] <file> [<file> ...] <path>'
    echo '  mkdir    [-t|--trace] [-f|--force] [-p|--parents] [-m|--mode'
    echo '           <mode>] [-o|--owner <owner>] [-g|--group <group>] <dir>'
    echo '           [<dir> ...]'
    echo '  platform [-F|--format <format>] [-S|--sep <string>] [-C|--conc'
    echo '           <string>] [-L|--lower] [-U|--upper] [-v|--verbose]'
    echo '           [-c|--concise] [-n|--no-newline] [-t|--type <type>]'
    echo '           [-V|--version] [-h|--help]'
    echo '  path     [-s|--suppress] [-r|--reverse] [-d|--dirname] [-b|--basename]'
    echo '           [-m|--magic] [-p|--path <path>] <str> [<str> ...]'
    echo ''
    echo 'Not available <cmd-name> (because module was not built-in):'
    echo '  mdate    [-n|--newline] [-z|--zero] [-s|--shorten] [-d|--digits]'
    echo '           [-f|--field-sep <str>] [-o|--order <spec>] <path>'
    echo '  table    [-F|--field-sep <sep>] [-w|--width <width>] [-c|--columns'
    echo '           <cols>] [-s|--strip <strip>] <str><sep><str>...'
    echo '  prop     [-p|--prefix <str>]'
    echo '  move     [-v|--verbose] [-t|--trace] [-e|--expand] [-p|--preserve]'
    echo '           <src-file> <dst-file>'
    echo '  mkln     [-t|--trace] [-f|--force] [-s|--symbolic] <src-path>'
    echo '           [<src-path> ...] <dst-path>'
    echo '  mkshadow [-v|--verbose] [-t|--trace] [-a|--all] <src-dir> <dst-dir>'
    echo '  fixperm  [-v|--verbose] [-t|--trace] <path> [<path> ...]'
    echo '  rotate   [-v|--verbose] [-t|--trace] [-f|--force] [-n|--num-files'
    echo '           <count>] [-s|--size <size>] [-c|--copy] [-r|--remove]'
    echo '           [-a|--archive-dir <dir>] [-z|--compress [<tool>:]<level>]'
    echo '           [-b|--background] [-d|--delay] [-p|--pad <len>] [-m|--mode'
    echo '           <mode>] [-o|--owner <owner>] [-g|--group <group>] [-M|--migrate'
    echo '           <cmd>] [-P|--prolog <cmd>] [-E|--epilog <cmd>] <file> [...]'
    echo '  tarball  [-t|--trace] [-v|--verbose] [-o|--output <tarball>]'
    echo '           [-c|--compress <prog>] [-d|--directory <dir>] [-u|--user'
    echo '           <user>] [-g|--group <group>] [-e|--exclude <pattern>]'
    echo '           <path> [<path> ...]'
    echo '  subst    [-v|--verbose] [-t|--trace] [-n|--nop] [-w|--warning]'
    echo '           [-q|--quiet] [-s|--stealth] [-i|--interactive] [-b|--backup'
    echo '           <ext>] [-e|--exec <cmd>] [-f|--file <cmd-file>] [<file>]'
    echo '           [...]'
    echo '  arx      [-t|--trace] [-C|--command <cmd>] <op> <archive> [<file>'
    echo '           ...]'
    echo '  slo      [-p|--prefix <str>] -- -L<dir> -l<lib> [-L<dir> -l<lib>'
    echo '           ...]'
    echo '  scpp     [-v|--verbose] [-p|--preserve] [-f|--filter <filter>]'
    echo '           [-o|--output <ofile>] [-t|--template <tfile>] [-M|--mark'
    echo '           <mark>] [-D|--define <dname>] [-C|--class <cname>]'
    echo '           <file> [<file> ...]'
    echo '  version  [-l|--language <lang>] [-n|--name <name>] [-p|--prefix'
    echo '           <prefix>] [-s|--set <version>] [-e|--edit] [-i|--increase'
    echo '           <knob>] [-d|--display <type>] <file>'
    echo ''
    exit 0
fi
if [ ".$1" = ".-v" ] || [ ".$1" = ".--version" ]; then
    echo "GNU shtool 2.0.8 (18-Jul-2008)"
    exit 0
fi
if [ ".$1" = ".-r" ] || [ ".$1" = ".--recreate" ]; then
    shtoolize -obuild/shtool echo install mkdir platform path
    exit 0
fi
if [ ".$1" = ".-d" ] || [ ".$1" = ".--debug" ]; then
    shift
    set -x
fi
name=`echo "$0" | sed -e 's;.*/\([^/]*\)$;\1;' -e 's;-sh$;;' -e 's;\.sh$;;'`
case "$name" in
    echo|install|mkdir|platform|path )
        #   implicit tool command selection
        tool="$name"
        ;;
    * )
        #   explicit tool command selection
        tool="$1"
        shift
        ;;
esac
arg_spec=""
opt_spec=""
gen_tmpfile=no

##
##  DISPATCH INTO SCRIPT PROLOG
##

case $tool in
    echo )
        str_tool="echo"
        str_usage="[-n|--newline] [-e|--expand] [<string> ...]"
        arg_spec="0+"
        opt_spec="n.e."
        opt_alias="n:newline,e:expand"
        opt_n=no
        opt_e=no
        ;;
    install )
        str_tool="install"
        str_usage="[-v|--verbose] [-t|--trace] [-d|--mkdir] [-c|--copy] [-C|--compare-copy] [-s|--strip] [-m|--mode <mode>] [-o|--owner <owner>] [-g|--group <group>] [-e|--exec <sed-cmd>] <file> [<file> ...] <path>"
        arg_spec="1+"
        opt_spec="v.t.d.c.C.s.m:o:g:e+"
        opt_alias="v:verbose,t:trace,d:mkdir,c:copy,C:compare-copy,s:strip,m:mode,o:owner,g:group,e:exec"
        opt_v=no
        opt_t=no
        opt_d=no
        opt_c=no
        opt_C=no
        opt_s=no
        opt_m="0755"
        opt_o=""
        opt_g=""
        opt_e=""
        ;;
    mkdir )
        str_tool="mkdir"
        str_usage="[-t|--trace] [-f|--force] [-p|--parents] [-m|--mode <mode>] [-o|--owner <owner>] [-g|--group <group>] <dir> [<dir> ...]"
        arg_spec="1+"
        opt_spec="t.f.p.m:o:g:"
        opt_alias="t:trace,f:force,p:parents,m:mode,o:owner,g:group"
        opt_t=no
        opt_f=no
        opt_p=no
        opt_m=""
        opt_o=""
        opt_g=""
        ;;
    platform )
        str_tool="platform"
        str_usage="[-F|--format <format>] [-S|--sep <string>] [-C|--conc <string>] [-L|--lower] [-U|--upper] [-v|--verbose] [-c|--concise] [-n|--no-newline] [-t|--type <type>] [-V|--version] [-h|--help]"
        arg_spec="0="
        opt_spec="F:S:C:L.U.v.c.n.t:d.V.h."
        opt_alias="F:format,S:sep,C:conc,L:lower,U:upper,v:verbose,c:consise,t:type,n:no-newline,V:version,h:help"
        opt_F="%{sp} (%{ap})"
        opt_S=" "
        opt_C="/"
        opt_L=no
        opt_U=no
        opt_t=""
        opt_v=no
        opt_c=no
        opt_n=no
        opt_V=no
        opt_h=no
        ;;
    path )
        str_tool="path"
        str_usage="[-s|--suppress] [-r|--reverse] [-d|--dirname] [-b|--basename] [-m|--magic] [-p|--path <path>] <str> [<str> ...]"
        gen_tmpfile=yes
        arg_spec="1+"
        opt_spec="s.r.d.b.m.p:"
        opt_alias="s:suppress,r:reverse,d:dirname,b:basename,m:magic,p:path"
        opt_s=no
        opt_r=no
        opt_d=no
        opt_b=no
        opt_m=no
        opt_p="$PATH"
        ;;
    -* )
        echo "$0:Error: unknown option \`$tool'" 2>&1
        echo "$0:Hint:  run \`$0 -h' for usage" 2>&1
        exit 1
        ;;
    * )
        echo "$0:Error: unknown command \`$tool'" 2>&1
        echo "$0:Hint:  run \`$0 -h' for usage" 2>&1
        exit 1
        ;;
esac

##
##  COMMON UTILITY CODE
##

#   commonly used ASCII values
ASC_TAB="	"
ASC_NL="
"

#   determine name of tool
if [ ".$tool" != . ]; then
    #   used inside shtool script
    toolcmd="$0 $tool"
    toolcmdhelp="shtool $tool"
    msgprefix="shtool:$tool"
else
    #   used as standalone script
    toolcmd="$0"
    toolcmdhelp="sh $0"
    msgprefix="$str_tool"
fi

#   parse argument specification string
eval `echo $arg_spec |\
      sed -e 's/^\([0-9]*\)\([+=]\)/arg_NUMS=\1; arg_MODE=\2/'`

#   parse option specification string
eval `echo h.$opt_spec |\
      sed -e 's/\([a-zA-Z0-9]\)\([.:+]\)/opt_MODE_\1=\2;/g'`

#   parse option alias string
eval `echo h:help,$opt_alias |\
      sed -e 's/-/_/g' -e 's/\([a-zA-Z0-9]\):\([^,]*\),*/opt_ALIAS_\2=\1;/g'`

#   interate over argument line
opt_PREV=''
while [ $# -gt 0 ]; do
    #   special option stops processing
    if [ ".$1" = ".--" ]; then
        shift
        break
    fi

    #   determine option and argument
    opt_ARG_OK=no
    if [ ".$opt_PREV" != . ]; then
        #   merge previous seen option with argument
        opt_OPT="$opt_PREV"
        opt_ARG="$1"
        opt_ARG_OK=yes
        opt_PREV=''
    else
        #   split argument into option and argument
        case "$1" in
            --[a-zA-Z0-9]*=*)
                eval `echo "x$1" |\
                      sed -e 's/^x--\([a-zA-Z0-9-]*\)=\(.*\)$/opt_OPT="\1";opt_ARG="\2"/'`
                opt_STR=`echo $opt_OPT | sed -e 's/-/_/g'`
                eval "opt_OPT=\${opt_ALIAS_${opt_STR}-${opt_OPT}}"
                ;;
            --[a-zA-Z0-9]*)
                opt_OPT=`echo "x$1" | cut -c4-`
                opt_STR=`echo $opt_OPT | sed -e 's/-/_/g'`
                eval "opt_OPT=\${opt_ALIAS_${opt_STR}-${opt_OPT}}"
                opt_ARG=''
                ;;
            -[a-zA-Z0-9]*)
                eval `echo "x$1" |\
                      sed -e 's/^x-\([a-zA-Z0-9]\)/opt_OPT="\1";/' \
                          -e 's/";\(.*\)$/"; opt_ARG="\1"/'`
                ;;
            -[a-zA-Z0-9])
                opt_OPT=`echo "x$1" | cut -c3-`
                opt_ARG=''
                ;;
            *)
                break
                ;;
        esac
    fi

    #   eat up option
    shift

    #   determine whether option needs an argument
    eval "opt_MODE=\$opt_MODE_${opt_OPT}"
    if [ ".$opt_ARG" = . ] && [ ".$opt_ARG_OK" != .yes ]; then
        if [ ".$opt_MODE" = ".:" ] || [ ".$opt_MODE" = ".+" ]; then
            opt_PREV="$opt_OPT"
            continue
        fi
    fi

    #   process option
    case $opt_MODE in
        '.' )
            #   boolean option
            eval "opt_${opt_OPT}=yes"
            ;;
        ':' )
            #   option with argument (multiple occurrences override)
            eval "opt_${opt_OPT}=\"\$opt_ARG\""
            ;;
        '+' )
            #   option with argument (multiple occurrences append)
            eval "opt_${opt_OPT}=\"\$opt_${opt_OPT}\${ASC_NL}\$opt_ARG\""
            ;;
        * )
            echo "$msgprefix:Error: unknown option: \`$opt_OPT'" 1>&2
            echo "$msgprefix:Hint:  run \`$toolcmdhelp -h' or \`man shtool' for details" 1>&2
            exit 1
            ;;
    esac
done
if [ ".$opt_PREV" != . ]; then
    echo "$msgprefix:Error: missing argument to option \`$opt_PREV'" 1>&2
    echo "$msgprefix:Hint:  run \`$toolcmdhelp -h' or \`man shtool' for details" 1>&2
    exit 1
fi

#   process help option
if [ ".$opt_h" = .yes ]; then
    echo "Usage: $toolcmdhelp $str_usage"
    exit 0
fi

#   complain about incorrect number of arguments
case $arg_MODE in
    '=' )
        if [ $# -ne $arg_NUMS ]; then
            echo "$msgprefix:Error: invalid number of arguments (exactly $arg_NUMS expected)" 1>&2
            echo "$msgprefix:Hint:  run \`$toolcmd -h' or \`man shtool' for details" 1>&2
            exit 1
        fi
        ;;
    '+' )
        if [ $# -lt $arg_NUMS ]; then
            echo "$msgprefix:Error: invalid number of arguments (at least $arg_NUMS expected)" 1>&2
            echo "$msgprefix:Hint:  run \`$toolcmd -h' or \`man shtool' for details" 1>&2
            exit 1
        fi
        ;;
esac

#   establish a temporary file on request
if [ ".$gen_tmpfile" = .yes ]; then
    #   create (explicitly) secure temporary directory
    if [ ".$TMPDIR" != . ]; then
        tmpdir="$TMPDIR"
    elif [ ".$TEMPDIR" != . ]; then
        tmpdir="$TEMPDIR"
    else
        tmpdir="/tmp"
    fi
    tmpdir="$tmpdir/.shtool.$$"
    ( umask 077
      rm -rf "$tmpdir" >/dev/null 2>&1 || true
      mkdir  "$tmpdir" >/dev/null 2>&1
      if [ $? -ne 0 ]; then
          echo "$msgprefix:Error: failed to create temporary directory \`$tmpdir'" 1>&2
          exit 1
      fi
    )

    #   create (implicitly) secure temporary file
    tmpfile="$tmpdir/shtool.tmp"
    touch "$tmpfile"
fi

#   utility function: map string to lower case
util_lower () {
    echo "$1" | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz'
}

#   utility function: map string to upper case
util_upper () {
    echo "$1" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
}

#   cleanup procedure
shtool_exit () {
    rc="$1"
    if [ ".$gen_tmpfile" = .yes ]; then
        rm -rf "$tmpdir" >/dev/null 2>&1 || true
    fi
    exit $rc
}

##
##  DISPATCH INTO SCRIPT BODY
##

case $tool in

echo )
    ##
    ##  echo -- Print string with optional construct expansion
    ##  Copyright (c) 1998-2008 Ralf S. Engelschall <rse@engelschall.com>
    ##

    text="$*"

    #   check for broken escape sequence expansion
    seo=''
    bytes=`echo '\1' | wc -c | awk '{ printf("%s", $1); }'`
    if [ ".$bytes" != .3 ]; then
        bytes=`echo -E '\1' | wc -c | awk '{ printf("%s", $1); }'`
        if [ ".$bytes" = .3 ]; then
            seo='-E'
        fi
    fi

    #   check for existing -n option (to suppress newline)
    minusn=''
    bytes=`echo -n 123 2>/dev/null | wc -c | awk '{ printf("%s", $1); }'`
    if [ ".$bytes" = .3 ]; then
        minusn='-n'
    fi

    #   determine terminal bold sequence
    term_bold=''
    term_norm=''
    if [ ".$opt_e" = .yes ] && [ ".`echo $text | grep '%[Bb]'`" != . ]; then
        case $TERM in
            #   for the most important terminal types we directly know the sequences
            xterm|xterm*|vt220|vt220*)
                term_bold=`awk 'BEGIN { printf("%c%c%c%c", 27, 91, 49, 109); }' </dev/null 2>/dev/null`
                term_norm=`awk 'BEGIN { printf("%c%c%c", 27, 91, 109); }' </dev/null 2>/dev/null`
                ;;
            vt100|vt100*|cygwin)
                term_bold=`awk 'BEGIN { printf("%c%c%c%c%c%c", 27, 91, 49, 109, 0, 0); }' </dev/null 2>/dev/null`
                term_norm=`awk 'BEGIN { printf("%c%c%c%c%c", 27, 91, 109, 0, 0); }' </dev/null 2>/dev/null`
                ;;
            #   for all others, we try to use a possibly existing `tput' or `tcout' utility
            * )
                paths=`echo $PATH | sed -e 's/:/ /g'`
                for tool in tput tcout; do
                    for dir in $paths; do
                        if [ -r "$dir/$tool" ]; then
                            for seq in bold md smso; do # 'smso' is last
                                bold="`$dir/$tool $seq 2>/dev/null`"
                                if [ ".$bold" != . ]; then
                                    term_bold="$bold"
                                    break
                                fi
                            done
                            if [ ".$term_bold" != . ]; then
                                for seq in sgr0 me rmso init reset; do # 'reset' is last
                                    norm="`$dir/$tool $seq 2>/dev/null`"
                                    if [ ".$norm" != . ]; then
                                        term_norm="$norm"
                                        break
                                    fi
                                done
                            fi
                            break
                        fi
                    done
                    if [ ".$term_bold" != . ] && [ ".$term_norm" != . ]; then
                        break;
                    fi
                done
                ;;
        esac
        if [ ".$term_bold" = . ] || [ ".$term_norm" = . ]; then
            echo "$msgprefix:Warning: unable to determine terminal sequence for bold mode" 1>&2
            term_bold=''
            term_norm=''
        fi
    fi

    #   determine user name
    username=''
    if [ ".$opt_e" = .yes ] && [ ".`echo $text | grep '%[uUgG]'`" != . ]; then
        username="`(id -un) 2>/dev/null`"
        if [ ".$username" = . ]; then
            str="`(id) 2>/dev/null`"
            if [ ".`echo $str | grep '^uid[ 	]*=[ 	]*[0-9]*('`" != . ]; then
                username=`echo $str | sed -e 's/^uid[ 	]*=[ 	]*[0-9]*(//' -e 's/).*$//'`
            fi
            if [ ".$username" = . ]; then
                username="$LOGNAME"
                if [ ".$username" = . ]; then
                    username="$USER"
                    if [ ".$username" = . ]; then
                        username="`(whoami) 2>/dev/null |\
                                   awk '{ printf("%s", $1); }'`"
                        if [ ".$username" = . ]; then
                            username="`(who am i) 2>/dev/null |\
                                       awk '{ printf("%s", $1); }'`"
                            if [ ".$username" = . ]; then
                                username='unknown'
                            fi
                        fi
                    fi
                fi
            fi
        fi
    fi

    #   determine user id
    userid=''
    if [ ".$opt_e" = .yes ] && [ ".`echo $text | grep '%U'`" != . ]; then
        userid="`(id -u) 2>/dev/null`"
        if [ ".$userid" = . ]; then
            userid="`(id -u ${username}) 2>/dev/null`"
            if [ ".$userid" = . ]; then
                str="`(id) 2>/dev/null`"
                if [ ".`echo $str | grep '^uid[ 	]*=[ 	]*[0-9]*('`" != . ]; then
                    userid=`echo $str | sed -e 's/^uid[ 	]*=[ 	]*//' -e 's/(.*$//'`
                fi
                if [ ".$userid" = . ]; then
                    userid=`(getent passwd ${username}) 2>/dev/null | \
                            sed -e 's/[^:]*:[^:]*://' -e 's/:.*$//'`
                    if [ ".$userid" = . ]; then
                        userid=`grep "^${username}:" /etc/passwd 2>/dev/null | \
                                sed -e 's/[^:]*:[^:]*://' -e 's/:.*$//'`
                        if [ ".$userid" = . ]; then
                            userid=`(ypmatch "${username}" passwd; nismatch "${username}" passwd) 2>/dev/null | \
                                    sed -e 'q' | sed -e 's/[^:]*:[^:]*://' -e 's/:.*$//'`
                            if [ ".$userid" = . ]; then
                                userid=`(nidump passwd . | grep "^${username}:") 2>/dev/null | \
                                        sed -e 's/[^:]*:[^:]*://' -e 's/:.*$//'`
                                if [ ".$userid" = . ]; then
                                    userid='?'
                                fi
                            fi
                        fi
                    fi
                fi
            fi
        fi
    fi

    #   determine (primary) group id
    groupid=''
    if [ ".$opt_e" = .yes ] && [ ".`echo $text | grep '%[gG]'`" != . ]; then
        groupid="`(id -g ${username}) 2>/dev/null`"
        if [ ".$groupid" = . ]; then
            str="`(id) 2>/dev/null`"
            if [ ".`echo $str | grep 'gid[ 	]*=[ 	]*[0-9]*('`" != . ]; then
                groupid=`echo $str | sed -e 's/^.*gid[ 	]*=[ 	]*//' -e 's/(.*$//'`
            fi
            if [ ".$groupid" = . ]; then
                groupid=`(getent passwd ${username}) 2>/dev/null | \
                         sed -e 's/[^:]*:[^:]*:[^:]*://' -e 's/:.*$//'`
                if [ ".$groupid" = . ]; then
                    groupid=`grep "^${username}:" /etc/passwd 2>/dev/null | \
                             sed -e 's/[^:]*:[^:]*:[^:]*://' -e 's/:.*$//'`
                    if [ ".$groupid" = . ]; then
                        groupid=`(ypmatch "${username}" passwd; nismatch "${username}" passwd) 2>/dev/null | \
                                 sed -e 'q' | sed -e 's/[^:]*:[^:]*:[^:]*://' -e 's/:.*$//'`
                        if [ ".$groupid" = . ]; then
                            groupid=`(nidump passwd . | grep "^${username}:") 2>/dev/null | \
                                     sed -e 's/[^:]*:[^:]*:[^:]*://' -e 's/:.*$//'`
                            if [ ".$groupid" = . ]; then
                                groupid='?'
                            fi
                        fi
                    fi
                fi
            fi
        fi
    fi

    #   determine (primary) group name
    groupname=''
    if [ ".$opt_e" = .yes ] && [ ".`echo $text | grep '%g'`" != . ]; then
        groupname="`(id -gn ${username}) 2>/dev/null`"
        if [ ".$groupname" = . ]; then
            str="`(id) 2>/dev/null`"
            if [ ".`echo $str | grep 'gid[ 	]*=[ 	]*[0-9]*('`" != . ]; then
                groupname=`echo $str | sed -e 's/^.*gid[ 	]*=[ 	]*[0-9]*(//' -e 's/).*$//'`
            fi
            if [ ".$groupname" = . ]; then
                groupname=`(getent group) 2>/dev/null | \
                           grep "^[^:]*:[^:]*:${groupid}:" | \
                           sed -e 's/:.*$//'`
                if [ ".$groupname" = . ]; then
                    groupname=`grep "^[^:]*:[^:]*:${groupid}:" /etc/group 2>/dev/null | \
                               sed -e 's/:.*$//'`
                    if [ ".$groupname" = . ]; then
                        groupname=`(ypcat group; niscat group) 2>/dev/null | \
                                   sed -e 'q' | grep "^[^:]*:[^:]*:${groupid}:" | \
                                   sed -e 's/:.*$//'`
                        if [ ".$groupname" = . ]; then
                            groupname=`(nidump group .) 2>/dev/null | \
                                       grep "^[^:]*:[^:]*:${groupid}:" | \
                                       sed -e 's/:.*$//'`
                            if [ ".$groupname" = . ]; then
                                groupname='?'
                            fi
                        fi
                    fi
                fi
            fi
        fi
    fi

    #   determine host and domain name
    hostname=''
    domainname=''
    if [ ".$opt_e" = .yes ] && [ ".`echo $text | grep '%h'`" != . ]; then
        hostname="`(uname -n) 2>/dev/null |\
                   awk '{ printf("%s", $1); }'`"
        if [ ".$hostname" = . ]; then
            hostname="`(hostname) 2>/dev/null |\
                       awk '{ printf("%s", $1); }'`"
            if [ ".$hostname" = . ]; then
                hostname='unknown'
            fi
        fi
        case $hostname in
            *.* )
                domainname=".`echo $hostname | cut -d. -f2-`"
                hostname="`echo $hostname | cut -d. -f1`"
                ;;
        esac
    fi
    if [ ".$opt_e" = .yes ] && [ ".`echo $text | grep '%d'`" != . ]; then
        if [ ".$domainname" = . ]; then
            if [ -f /etc/resolv.conf ]; then
                domainname="`grep '^[ 	]*domain' /etc/resolv.conf | sed -e 'q' |\
                             sed -e 's/.*domain//' \
                                 -e 's/^[ 	]*//' -e 's/^ *//' -e 's/^	*//' \
                                 -e 's/^\.//' -e 's/^/./' |\
                             awk '{ printf("%s", $1); }'`"
                if [ ".$domainname" = . ]; then
                    domainname="`grep '^[ 	]*search' /etc/resolv.conf | sed -e 'q' |\
                                 sed -e 's/.*search//' \
                                     -e 's/^[ 	]*//' -e 's/^ *//' -e 's/^	*//' \
                                     -e 's/ .*//' -e 's/	.*//' \
                                     -e 's/^\.//' -e 's/^/./' |\
                                 awk '{ printf("%s", $1); }'`"
                fi
            fi
        fi
    fi

    #   determine current time
    time_day=''
    time_month=''
    time_year=''
    time_monthname=''
    if [ ".$opt_e" = .yes ] && [ ".`echo $text | grep '%[DMYm]'`" != . ]; then
        time_day=`date '+%d'`
        time_month=`date '+%m'`
        time_year=`date '+%Y' 2>/dev/null`
        if [ ".$time_year" = . ]; then
            time_year=`date '+%y'`
            case $time_year in
                [5-9][0-9]) time_year="19$time_year" ;;
                [0-4][0-9]) time_year="20$time_year" ;;
            esac
        fi
        case $time_month in
            1|01) time_monthname='Jan' ;;
            2|02) time_monthname='Feb' ;;
            3|03) time_monthname='Mar' ;;
            4|04) time_monthname='Apr' ;;
            5|05) time_monthname='May' ;;
            6|06) time_monthname='Jun' ;;
            7|07) time_monthname='Jul' ;;
            8|08) time_monthname='Aug' ;;
            9|09) time_monthname='Sep' ;;
              10) time_monthname='Oct' ;;
              11) time_monthname='Nov' ;;
              12) time_monthname='Dec' ;;
        esac
    fi

    #   expand special ``%x'' constructs
    if [ ".$opt_e" = .yes ]; then
        text=`echo $seo "$text" |\
              sed -e "s/%B/${term_bold}/g" \
                  -e "s/%b/${term_norm}/g" \
                  -e "s/%u/${username}/g" \
                  -e "s/%U/${userid}/g" \
                  -e "s/%g/${groupname}/g" \
                  -e "s/%G/${groupid}/g" \
                  -e "s/%h/${hostname}/g" \
                  -e "s/%d/${domainname}/g" \
                  -e "s/%D/${time_day}/g" \
                  -e "s/%M/${time_month}/g" \
                  -e "s/%Y/${time_year}/g" \
                  -e "s/%m/${time_monthname}/g" 2>/dev/null`
    fi

    #   create output
    if [ .$opt_n = .no ]; then
        echo $seo "$text"
    else
        #   the harder part: echo -n is best, because
        #   awk may complain about some \xx sequences.
        if [ ".$minusn" != . ]; then
            echo $seo $minusn "$text"
        else
            echo dummy | awk '{ printf("%s", TEXT); }' TEXT="$text"
        fi
    fi

    shtool_exit 0
    ;;

install )
    ##
    ##  install -- Install a program, script or datafile
    ##  Copyright (c) 1997-2008 Ralf S. Engelschall <rse@engelschall.com>
    ##

    #   special case: "shtool install -d <dir> [...]" internally
    #   maps to "shtool mkdir -f -p -m 755 <dir> [...]"
    if [ "$opt_d" = yes ]; then
        cmd="$0 mkdir -f -p -m 755"
        if [ ".$opt_o" != . ]; then
            cmd="$cmd -o '$opt_o'"
        fi
        if [ ".$opt_g" != . ]; then
            cmd="$cmd -g '$opt_g'"
        fi
        if [ ".$opt_v" = .yes ]; then
            cmd="$cmd -v"
        fi
        if [ ".$opt_t" = .yes ]; then
            cmd="$cmd -t"
        fi
        for dir in "$@"; do
            eval "$cmd $dir" || shtool_exit $?
        done
        shtool_exit 0
    fi

    #   determine source(s) and destination
    argc=$#
    srcs=""
    while [ $# -gt 1 ]; do
        srcs="$srcs $1"
        shift
    done
    dstpath="$1"

    #   type check for destination
    dstisdir=0
    if [ -d $dstpath ]; then
        dstpath=`echo "$dstpath" | sed -e 's:/$::'`
        dstisdir=1
    fi

    #   consistency check for destination
    if [ $argc -gt 2 ] && [ $dstisdir = 0 ]; then
        echo "$msgprefix:Error: multiple sources require destination to be directory" 1>&2
        shtool_exit 1
    fi

    #   iterate over all source(s)
    for src in $srcs; do
        dst=$dstpath

        #   if destination is a directory, append the input filename
        if [ $dstisdir = 1 ]; then
            dstfile=`echo "$src" | sed -e 's;.*/\([^/]*\)$;\1;'`
            dst="$dst/$dstfile"
        fi

        #   check for correct arguments
        if [ ".$src" = ".$dst" ]; then
            echo "$msgprefix:Warning: source and destination are the same - skipped" 1>&2
            continue
        fi
        if [ -d "$src" ]; then
            echo "$msgprefix:Warning: source \`$src' is a directory - skipped" 1>&2
            continue
        fi

        #   make a temp file name in the destination directory
        dsttmp=`echo $dst |\
                sed -e 's;[^/]*$;;' -e 's;\(.\)/$;\1;' -e 's;^$;.;' \
                    -e "s;\$;/#INST@$$#;"`

        #   verbosity
        if [ ".$opt_v" = .yes ]; then
            echo "$src -> $dst" 1>&2
        fi

        #   copy or move the file name to the temp name
        #   (because we might be not allowed to change the source)
        if [ ".$opt_C" = .yes ]; then
            opt_c=yes
        fi
        if [ ".$opt_c" = .yes ]; then
            if [ ".$opt_t" = .yes ]; then
                echo "cp $src $dsttmp" 1>&2
            fi
            cp "$src" "$dsttmp" || shtool_exit $?
        else
            if [ ".$opt_t" = .yes ]; then
                echo "mv $src $dsttmp" 1>&2
            fi
            mv "$src" "$dsttmp" || shtool_exit $?
        fi

        #   adjust the target file
        if [ ".$opt_e" != . ]; then
            sed='sed'
            OIFS="$IFS"; IFS="$ASC_NL"; set -- $opt_e; IFS="$OIFS"
            for e
            do
                sed="$sed -e '$e'"
            done
            cp "$dsttmp" "$dsttmp.old"
            chmod u+w $dsttmp
            eval "$sed <$dsttmp.old >$dsttmp" || shtool_exit $?
            rm -f $dsttmp.old
        fi
        if [ ".$opt_s" = .yes ]; then
            if [ ".$opt_t" = .yes ]; then
                echo "strip $dsttmp" 1>&2
            fi
            strip $dsttmp || shtool_exit $?
        fi
        if [ ".$opt_o" != . ]; then
            if [ ".$opt_t" = .yes ]; then
                echo "chown $opt_o $dsttmp" 1>&2
            fi
            chown $opt_o $dsttmp || shtool_exit $?
        fi
        if [ ".$opt_g" != . ]; then
            if [ ".$opt_t" = .yes ]; then
                echo "chgrp $opt_g $dsttmp" 1>&2
            fi
            chgrp $opt_g $dsttmp || shtool_exit $?
        fi
        if [ ".$opt_m" != ".-" ]; then
            if [ ".$opt_t" = .yes ]; then
                echo "chmod $opt_m $dsttmp" 1>&2
            fi
            chmod $opt_m $dsttmp || shtool_exit $?
        fi

        #   determine whether to do a quick install
        #   (has to be done _after_ the strip was already done)
        quick=no
        if [ ".$opt_C" = .yes ]; then
            if [ -r $dst ]; then
                if cmp -s "$src" "$dst"; then
                    quick=yes
                fi
            fi
        fi

        #   finally, install the file to the real destination
        if [ $quick = yes ]; then
            if [ ".$opt_t" = .yes ]; then
                echo "rm -f $dsttmp" 1>&2
            fi
            rm -f $dsttmp
        else
            if [ ".$opt_t" = .yes ]; then
                echo "rm -f $dst && mv $dsttmp $dst" 1>&2
            fi
            rm -f $dst && mv $dsttmp $dst
        fi
    done

    shtool_exit 0
    ;;

mkdir )
    ##
    ##  mkdir -- Make one or more directories
    ##  Copyright (c) 1996-2008 Ralf S. Engelschall <rse@engelschall.com>
    ##

    errstatus=0
    for p in ${1+"$@"}; do
        #   if the directory already exists...
        if [ -d "$p" ]; then
            if [ ".$opt_f" = .no ] && [ ".$opt_p" = .no ]; then
                echo "$msgprefix:Error: directory already exists: $p" 1>&2
                errstatus=1
                break
            else
                continue
            fi
        fi
        #   if the directory has to be created...
        if [ ".$opt_p" = .no ]; then
            if [ ".$opt_t" = .yes ]; then
                echo "mkdir $p" 1>&2
            fi
            mkdir $p || errstatus=$?
            if [ ".$opt_o" != . ]; then
                if [ ".$opt_t" = .yes ]; then
                    echo "chown $opt_o $p" 1>&2
                fi
                chown $opt_o $p || errstatus=$?
            fi
            if [ ".$opt_g" != . ]; then
                if [ ".$opt_t" = .yes ]; then
                    echo "chgrp $opt_g $p" 1>&2
                fi
                chgrp $opt_g $p || errstatus=$?
            fi
            if [ ".$opt_m" != . ]; then
                if [ ".$opt_t" = .yes ]; then
                    echo "chmod $opt_m $p" 1>&2
                fi
                chmod $opt_m $p || errstatus=$?
            fi
        else
            #   the smart situation
            set fnord `echo ":$p" |\
                       sed -e 's/^:\//%/' \
                           -e 's/^://' \
                           -e 's/\// /g' \
                           -e 's/^%/\//'`
            shift
            pathcomp=''
            for d in ${1+"$@"}; do
                pathcomp="$pathcomp$d"
                case "$pathcomp" in
                    -* ) pathcomp="./$pathcomp" ;;
                esac
                if [ ! -d "$pathcomp" ]; then
                    if [ ".$opt_t" = .yes ]; then
                        echo "mkdir $pathcomp" 1>&2
                    fi
					# See https://bugs.php.net/51076
					# The fix is from Debian who have sent it
					# upstream, too; but upstream seems dead.
                    mkdir $pathcomp || {
                        _errstatus=$?
                        [ -d "$pathcomp" ] || errstatus=${_errstatus}
                        unset _errstatus
                    }
                    if [ ".$opt_o" != . ]; then
                        if [ ".$opt_t" = .yes ]; then
                            echo "chown $opt_o $pathcomp" 1>&2
                        fi
                        chown $opt_o $pathcomp || errstatus=$?
                    fi
                    if [ ".$opt_g" != . ]; then
                        if [ ".$opt_t" = .yes ]; then
                            echo "chgrp $opt_g $pathcomp" 1>&2
                        fi
                        chgrp $opt_g $pathcomp || errstatus=$?
                    fi
                    if [ ".$opt_m" != . ]; then
                        if [ ".$opt_t" = .yes ]; then
                            echo "chmod $opt_m $pathcomp" 1>&2
                        fi
                        chmod $opt_m $pathcomp || errstatus=$?
                    fi
                fi
                pathcomp="$pathcomp/"
            done
        fi
    done

    shtool_exit $errstatus
    ;;

platform )
    ##
    ##  platform -- Platform Identification Utility
    ##  Copyright (c) 2003-2008 Ralf S. Engelschall <rse@engelschall.com>
    ##

    #   option post-processing
    if [ ".$opt_t" != . ]; then
        case "$opt_t" in
            binary )
                #   binary package id (OpenPKG RPM)
                opt_F="%<ap>-%<sp>"
                opt_L=yes
                opt_S=""
                opt_C="+"
                ;;
            build )
                #   build time checking (OpenPKG RPM)
                opt_F="%<at>-%<st>"
                opt_L=yes
                opt_S=""
                opt_C="+"
                ;;
            gnu )
                #   GNU config.guess style <arch>-<vendor>-<os><osversion>
                opt_F="%<at>-unknown-%<st>"
                opt_L=yes
                opt_S=""
                opt_C="+"
                ;;
            web )
                #   non-whitespace HTTP Server-header id
                opt_F="%<sp>-%<ap>"
                opt_S="/"
                opt_C="+"
                ;;
            summary)
                #   human readable verbose summary information
                opt_F="Class:      %[sc] (%[ac])\\nProduct:    %[sp] (%[ap])\\nTechnology: %[st] (%[at])"
                opt_S=" "
                opt_C="/"
                ;;
            all-in-one )
                #   full-table all-in-one information
                opt_F=""
                opt_F="${opt_F}concise architecture class:      %<ac>\\n"
                opt_F="${opt_F}regular architecture class:      %{ac}\\n"
                opt_F="${opt_F}verbose architecture class:      %[ac]\\n"
                opt_F="${opt_F}concise architecture product:    %<ap>\\n"
                opt_F="${opt_F}regular architecture product:    %{ap}\\n"
                opt_F="${opt_F}verbose architecture product:    %[ap]\\n"
                opt_F="${opt_F}concise architecture technology: %<at>\\n"
                opt_F="${opt_F}regular architecture technology: %{at}\\n"
                opt_F="${opt_F}verbose architecture technology: %[at]\\n"
                opt_F="${opt_F}concise system class:            %<sc>\\n"
                opt_F="${opt_F}regular system class:            %{sc}\\n"
                opt_F="${opt_F}verbose system class:            %[sc]\\n"
                opt_F="${opt_F}concise system product:          %<sp>\\n"
                opt_F="${opt_F}regular system product:          %{sp}\\n"
                opt_F="${opt_F}verbose system product:          %[sp]\\n"
                opt_F="${opt_F}concise system technology:       %<st>\\n"
                opt_F="${opt_F}regular system technology:       %{st}\\n"
                opt_F="${opt_F}verbose system technology:       %[st]"
                ;;
            * )
                echo "$msgprefix:Error: invalid type \`$opt_t'" 1>&2
                exit 1
                ;;
        esac
    fi

    #   assemble initial platform information
    UNAME_MACHINE=`(uname -m) 2>/dev/null` ||\
    UNAME_MACHINE=`(uname -p) 2>/dev/null` ||\
    UNAME_MACHINE='unknown'
    UNAME_SYSTEM=`(uname -s) 2>/dev/null`  ||\
    UNAME_SYSTEM='unknown'
    UNAME_RELEASE=`(uname -r) 2>/dev/null` ||\
    UNAME_RELEASE=`(uname -v) 2>/dev/null` ||\
    UNAME_RELEASE='unknown'

    UNAME="${UNAME_MACHINE}:${UNAME_SYSTEM}:${UNAME_RELEASE}"

    AC=""; AP=""; AT=""
    SC=""; SP=""; ST=""

    #    dispatch into platform specific sections
    case "${UNAME}" in

        #   FreeBSD
        *:FreeBSD:* )
            #   determine architecture
            AC="${UNAME_MACHINE}"
            case "${AC}" in
                i386 ) AC="iX86" ;;
            esac
            AP="${AC}"
            AT="${AP}"
            if [ ".${AT}" = ".iX86" ]; then
                case "`(/sbin/sysctl -n hw.model) 2>&1`" in
                    *"Xeon"* | *"Pentium Pro"* | *"Cyrix 6x86MX"* | *"Pentium II"* | *"Pentium III"* | *"Pentium 4"* | *"Celeron"* ) AT="i686" ;;
                    *"Pentium"* ) AT="i586" ;; *"i486[SD]X"* | *"Cyrix 486"* | *"Cyrix [56]x86"* | *"Blue Lightning" | *"Cyrix 486S/DX" ) AT="i486" ;;
                    *"i386[SD]X"* | *"NexGen 586"* ) AT="i386" ;;
                esac
            fi
            #   determine system
            r=`echo "${UNAME_RELEASE}" |\
               sed -e 's;[()];;' -e 's/\(-.*\)$/[\1]/'`
            ST="FreeBSD ${r}"
            SP="${ST}"
            case "${r}" in
                1.* ) SC="4.3BSD" ;;
                *   ) SC="4.4BSD" ;;
            esac
            ;;

        #   NetBSD
        *:NetBSD:* )
            #   determine architecture
            AT="${UNAME_MACHINE}"
            AP="${AT}"
            case "${AP}" in
                i[3-6]86 ) AP="iX86" ;;
            esac
            AC="${AP}"
            #   determine system
            r=`echo "${UNAME_RELEASE}" | sed -e 's/\([-_].*\)$/[\1]/'`
            ST="NetBSD ${r}"
            SP="${ST}"
            case "${r}" in
                0.* ) SC="4.3BSD" ;;
                *   ) SC="4.4BSD" ;;
            esac
            ;;

        #   OpenBSD
        *:OpenBSD:* )
            #   determine architecture
            AT="${UNAME_MACHINE}"
            AP="${AT}"
            case "${AP}" in
                i[3-6]86 ) AP="iX86" ;;
            esac
            AC="${AP}"
            #   determine system
            r=`echo "${UNAME_RELEASE}" | sed -e 's/\([-_].*\)$/[\1]/'`
            ST="OpenBSD ${r}"
            SP="${ST}"
            SC="4.4BSD"
            ;;

        #   DragonFly BSD
        *:DragonFly:* )
            #   determine architecture
            AT="${UNAME_MACHINE}"
            AP="${AT}"
            case "${AP}" in
                i[3-6]86 ) AP="iX86" ;;
            esac
            AC="${AP}"
            #   determine system
            r=`echo "${UNAME_RELEASE}" | sed -e 's/\([-_].*\)$/[\1]/'`
            ST="DragonFly ${r}"
            SP="${ST}"
            SC="4.4BSD"
            ;;

        #   GNU/Linux
        *:Linux:* )
            #   determine architecture
            AT="${UNAME_MACHINE}"
            case "${AT}" in
               ia64     ) AT="IA64"   ;;
               x86_64   ) AT='AMD64'  ;;
               parisc   ) AT="HPPA32" ;;
               parisc64 ) AT="HPPA64" ;;
            esac
            AP="${AT}"
            case "${AP}" in
               i[3-6]86 ) AP='iX86' ;;
            esac
            AC="${AP}"
            #   determine system
            v_kern=`echo "${UNAME_RELEASE}" |\
                sed -e 's/^\([0-9][0-9]*\.[0-9][0-9]*\).*/\1/'`
            v_libc=`(strings /lib/libc.so.* | grep '^GLIBC_' | sed -e 's/^GLIBC_//' |\
                env -i sort -n | sed -n -e '$p' | sed -e 's/^\([0-9][0-9]*\.[0-9][0-9]*\).*/\1/') 2>/dev/null`
            ST="GNU/<Linux >${v_libc}/<${v_kern}>"
            if [ -f /etc/lsb-release ]; then
                eval `( . /etc/lsb-release
                    echo "SC=\"LSB${LSB_VERSION}\""
                    if [ ".${DISTRIB_ID}" != . -a ".${DISTRIB_RELEASE}" != . ]; then
                        echo "SP=\"${DISTRIB_ID} ${DISTRIB_RELEASE}\""
                    fi
                ) 2>/dev/null`
            fi
            if [ ".$SP" = . ]; then
                for tagfile in x \
                    `cd /etc && \
                    /bin/ls *[_-]release *[_-]version 2>/dev/null | env -i sort | \
                    sed -e '/^redhat-release$/d' -e '/^lsb-release$/d'; \
                    echo redhat-release lsb-release`
                do
                    [ ".${tagfile}" = .x ] && continue
                    [ ! -f "/etc/${tagfile}" ] && continue
                    n=`echo ${tagfile} | sed -e 's/[_-]release$//' -e 's/[_-]version$//'`
                    v=`(grep VERSION /etc/${tagfile}; cat /etc/${tagfile}) | grep '[0-9]' | sed -e 'q' |\
                       sed -e 's/^/#/' \
                           -e 's/^#[^0-9]*\([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*$/\1/' \
                           -e 's/^#[^0-9]*\([0-9][0-9]*\.[0-9][0-9]*\).*$/\1/' \
                           -e 's/^#[^0-9]*\([0-9][0-9]*\).*$/\1/' \
                           -e 's/^#.*$//'`
                    case "`util_lower ${n}`" in
                        redhat )
                            if [ ".`egrep '(Red Hat Enterprise Linux|CentOS)' /etc/${tagfile}`" != . ]; then
                                n="<R>ed <H>at <E>nterprise <L>inux"
                            else
                                n="<R>ed <H>at <L>inux"
                            fi
                            ;;
                        debian             ) n="Debian[ GNU/Linux]"        ;;
                        ubuntu             ) n="Ubuntu[ GNU/Linux]"        ;;
                        fedora             ) n="<Fedora> Core[ GNU/Linux]" ;;
                        suse               ) n="[Novell ]SUSE[ Linux]"     ;;
                        mandrake*|mandriva ) n="Mandriva[ Linux]"          ;;
                        gentoo             ) n="Gentoo[ GNU/Linux]"        ;;
                        slackware          ) n="Slackware[ Linux]"         ;;
                        turbolinux         ) n="TurboLinux"                ;;
                        unitedlinux        ) n="UnitedLinux"               ;;
                        *                  ) n="${n}[ GNU/Linux]"          ;;
                    esac
                    case "$n" in
                        *"<"*">"* ) SP="$n <$v>" ;;
                        *         ) SP="$n $v"   ;;
                    esac
                    break
                done
            fi
            [ ".$SP" = . ] && SP="${ST}"
            [ ".$SC" = . ] && SC="LSB"
            ;;

        #   Sun Solaris
        *:SunOS:* )
            #   determine architecture
            AT="${UNAME_MACHINE}"
            case "${AT}" in
                i86pc )
                    AT="iX86"
                    case "`(/bin/isainfo -k) 2>&1`" in
                        amd64 ) AT="AMD64" ;;
                    esac
                    ;;
            esac
            AP="${AT}"
            case "${AP}" in
                sun4[cdm] ) AP="SPARC32" ;;
                sun4[uv]  ) AP="SPARC64" ;;
                sun4*     ) AP="SPARC"   ;;
            esac
            AC="${AP}"
            case "${AC}" in
                SPARC* ) AC="SPARC" ;;
            esac
            #   determine system
            ST="[Sun ]SunOS ${UNAME_RELEASE}"
            v=`echo "${UNAME_RELEASE}" |\
               sed -e 's;^4\.;1.;' \
                   -e 's;^5\.\([0-6]\)[^0-9]*$;2.\1;' \
                   -e 's;^5\.\([0-9][0-9]*\).*;\1;'`
            SP="[Sun ]Solaris $v"
            case "${UNAME_RELEASE}" in
                4.* ) SC="4.3BSD" ;;
                5.* ) SC="SVR4"   ;;
            esac
            ;;

        #   SCO UnixWare
        *:UnixWare:* )
            #   determine architecture
            AT="${UNAME_MACHINE}"
            case "${AT}" in
                i[3-6]86 | ix86at ) AT="iX86" ;;
            esac
            AP="${AT}"
            #   determine system
            v=`/sbin/uname -v`
            ST="[SCO ]UnixWare ${v}"
            SP="${ST}"
            SC="SVR${UNAME_RELEASE}"
            ;;

        #   QNX
        *:QNX:* )
            #   determine architecture
            AT="${UNAME_MACHINE}"
            case "${AT}" in
                x86pc ) AT="iX86" ;;
            esac
            AP="${AT}"
            #   determine system
            v="${UNAME_RELEASE}"
            ST="QNX[ Neutrino RTOS] ${v}"
            v=`echo "${v}" | sed -e 's;^\([0-9][0-9]*\.[0-9][0-9]*\).*$;\1;'`
            SP="QNX[ Neutrino RTOS] ${v}"
            SC="QNX"
            ;;

        #   SGI IRIX
        *:IRIX*:* )
            #   determine architecture
            AT="${UNAME_MACHINE}"
            AP="${AT}"
            case "${AP}:${UNAME_SYSTEM}" in
                IP*:IRIX64 ) AP="MIPS64" ;;
                IP*:*      ) AP="MIPS"   ;;
            esac
            AC="${AP}"
            #   determine system
            v=`(/bin/uname -R || /bin/uname -r) 2>/dev/null | sed -e 's;[0-9.]* ;;'`
            ST="[SGI ]IRIX ${v}"
            v="${UNAME_RELEASE}"
            SP="[SGI ]IRIX ${v}"
            SC="4.2BSD/SVR3"
            ;;

        #   HP HP-UX
        *:HP-UX:* )
            #   determine architecture
            AT="${UNAME_MACHINE}"
            case "${AT}" in
                ia64 ) AT="IA64" ;;
                9000/[34]?? ) AT=M68K ;;
                9000/[678][0-9][0-9])
                    sc_cpu_version=`/usr/bin/getconf SC_CPU_VERSION 2>/dev/null`
                    sc_kernel_bits=`/usr/bin/getconf SC_KERNEL_BITS 2>/dev/null`
                    case "${sc_cpu_version}" in
                        523 ) AT="HPPA1.0" ;;
                        528 ) AT="HPPA1.1" ;;
                        532 ) AT="HPPA2.0"
                            case "${sc_kernel_bits}" in
                                32 ) AT="${AT}n" ;;
                                64 ) AT="${AT}w" ;;
                            esac
                            ;;
                    esac
                    ;;
            esac
            AP="${AT}"
            case "${AP}" in
                HPPA* ) AP="HPPA" ;;
            esac
            AC="${AP}"
            #   determine system
            v=`echo "${UNAME_RELEASE}" | sed -e 's;^[^0-9]*;;'`
            ST="[HP ]<HP>-<UX ${v}>"
            SP="${ST}"
            case "${v}" in
                10.*   ) SC="SVR4.2" ;;
                [7-9]* ) SC="SVR4"   ;;
            esac
            ;;

        #   HP Tru64 (OSF1)
        *:OSF1:* )
            #   determine architecture
            AP="${UNAME_MACHINE}"
            case "${AP}" in
                alpha ) AP="Alpha" ;;
            esac
            alpha_type=`(/usr/sbin/psrinfo -v) 2>/dev/null |\
                sed -n -e 's/^.*The alpha \([^ ][^ ]*\).*processor.*$/\1/p' | sed -e 'q'`
            AT="${AP}${alpha_type}"
            AC="${AP}"
            #   determine system
            v=`echo "${UNAME_RELEASE}" | sed -e 's;^[VTX];;'`
            ST="[HP ]Tru64 ${v}"
            SP="${ST}"
            SC="OSF1"
            ;;

        #   IBM AIX
        *:AIX:* )
            #   determine architecture
            cpu_arch=RS6000
            if [ -x /usr/sbin/lsdev -a -x /usr/sbin/lsattr ]; then
                cpu_id=`/usr/sbin/lsdev -C -c processor -S available | sed 1q | awk '{ print $1 }'`
                if [ ".`/usr/sbin/lsattr -El ${cpu_id} | grep -i powerpc`" != . ]; then
                    cpu_arch=PPC
                fi
            elif [ -d /QOpenSys ]; then
                #   IBM i5/OS (aka OS/400) with PASE (Portable Application Solutions Environment)
                cpu_arch=PPC
            fi
            if [ -x /usr/bin/oslevel ]; then
                os_level=`/usr/bin/oslevel`
            else
                os_level="`uname -v`.`uname -r`"
            fi
            os_level=`echo "${os_level}" |\
                      sed -e 's;^\([0-9][0-9]*\.[0-9][0-9]*\)\(\.[0-9][0-9]*\)\(.*\)$;<\1>\2[\3];' \
                          -e 's;^\([0-9][0-9]*\.[0-9][0-9]*\)\(.*\)$;<\1>\2;'`
            AT="${cpu_arch}"
            AP="${AT}"
            AC="${AP}"
            #   determine system
            ST="[IBM ]<AIX >${os_level}"
            SP="${ST}"
            case "${os_level}" in
                [12]* ) SC="SVR2" ;;
                *     ) SC="SVR4" ;;
            esac
            ;;

        #   Apple Mac OS X (Darwin)
        *:Darwin:* )
            #   determine architecture
            AT="`uname -p`"
            case "${AT}" in
                powerpc ) AT="PPC" ;;
            esac
            AP="${AT}"
            case "${AP}" in
                i?86 ) AP="iX86" ;;
            esac
            AC="${AP}"
            #   determine system
            unset v1; unset v2; unset v3
            eval `echo "${UNAME_RELEASE}" |\
                  sed -e 's/^/#/' \
                      -e 's/^#\([0-9][0-9]*\)\.\([0-9][0-9]*\)\.\([0-9][0-9]*\).*$/v1="\1"; v2="\2"; v3="\3"/' \
                      -e 's/^#\([0-9][0-9]*\)\.\([0-9][0-9]*\).*$/v1="\1"; v2="\2"/' \
                      -e 's/^#\([0-9][0-9]*\).*$/v1="\1"/' \
                      -e 's/^#.*$/v1="0"/'`
            ST="[Apple ]<${UNAME_SYSTEM} ${v1}>${v2+.$v2}${v3+[.$v3]}"
            SP="$ST"
            v="`(sw_vers) 2>/dev/null | grep 'ProductVersion:' | sed -e 's/^ProductVersion:[^0-9]*\([0-9][0-9.]*\).*$/\1/'`"
            if [ ".$v" = . ]; then
                for name in System Server; do
                    if [ -f /System/Library/CoreServices/${name}Version.plist ]; then
                        v=`(defaults read "/System/Library/CoreServices/${name}Version" "ProductVersion") 2>/dev/null`
                        [ ".$v" != . ] && break
                    fi
                done
            fi
            if [ ".$v" != . ]; then
                unset v1; unset v2; unset v3
                eval `echo "${v}" |\
                      sed -e 's/^/#/' \
                          -e 's/^#\([0-9][0-9]*\)\.\([0-9][0-9]*\)\.\([0-9][0-9]*\).*$/v1="\1"; v2="\2"; v3="\3"/' \
                          -e 's/^#\([0-9][0-9]*\)\.\([0-9][0-9]*\).*$/v1="\1"; v2="\2"/' \
                          -e 's/^#\([0-9][0-9]*\).*$/v1="\1"/' \
                          -e 's/^#.*$/v1="0"/'`
                SP="[Apple ]Mac OS X ${v1}${v2+.$v2}${v3+[.$v3]}"
            fi
            SC="4.4BSD/Mach3.0"
            ;;

        #   Windows/Cygwin
        *:CYGWIN*:* )
            #   determine architecture
            AT="`uname -m`"
            AP="${AT}"
            case "${AP}" in
                i?86 ) AP="iX86" ;;
            esac
            AC="${AP}"
            #   determine system
            unset v1; unset v2; unset v3
            eval `echo "${UNAME_RELEASE}" |\
                  sed -e 's/^/#/' \
                      -e 's/^#\([0-9][0-9]*\)\.\([0-9][0-9]*\)\.\([0-9][0-9]*\).*$/v1="\1"; v2="\2"; v3="\3"/' \
                      -e 's/^#\([0-9][0-9]*\)\.\([0-9][0-9]*\).*$/v1="\1"; v2="\2"/' \
                      -e 's/^#\([0-9][0-9]*\).*$/v1="\1"/' \
                      -e 's/^#.*$/v1="0"/'`
            ST="Cygwin ${v1}${v2+.$v2}${v3+[.$v3]}"
            SP="$ST"
            SC="Windows"
            v=`echo "${UNAME_SYSTEM}" | sed -e 's/^CYGWIN_NT-//' |\
               sed -e 's/^/#/' -e 's/^#\([0-9][0-9]*\.[0-9][0-9]*\).*$/\1/' -e 's/^#.*$//'`
            case "$v" in
                4.0 ) SC="$SC[ NT]"    ;;
                5.0 ) SC="$SC[ 2000]"  ;;
                5.1 ) SC="$SC[ XP]"    ;;
                6.0 ) SC="$SC[ Vista]" ;;
            esac
            ;;

        #   TODO ...ADD YOUR NEW PLATFORM CHECK HERE... TODO
        # *:XXX:* )
        #   ...
        #   ;;

        #   ...A STILL UNKNOWN PLATFORM...
        * )
            AT=`echo "${UNAME_MACHINE}" | sed -e "s; ;${opt_C};g"`
            AP="${AT}"
            AC="${AP}"
            v=`echo "${UNAME_RELEASE}" |\
               sed -e 's/^/#/' \
                   -e 's/^#[^0-9]*\([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*$/\1/' \
                   -e 's/^#[^0-9]*\([0-9][0-9]*\.[0-9][0-9]*\).*$/\1/' \
                   -e 's/^#[^0-9]*\([0-9][0-9]*\).*$/\1/' \
                   -e 's/^#.*$/?/'`
            ST="${UNAME_SYSTEM} ${v}"
            SP="${ST}"
            SC="${SP}"
            ;;

    esac

    #   provide fallback values
    [ ".$AT" = . ] && AT="${AP:-${AC}}"
    [ ".$AP" = . ] && AP="${AT:-${AC}}"
    [ ".$AC" = . ] && AC="${AP:-${AT}}"
    [ ".$ST" = . ] && ST="${SP:-${SC}}"
    [ ".$SP" = . ] && SP="${ST:-${SC}}"
    [ ".$SC" = . ] && SC="${SP:-${ST}}"

    #   support explicit enforced verbose/concise output
    if [ ".$opt_v" = .yes ]; then
        opt_F=`echo ":$opt_F" | sed -e 's/^://' -e 's/%\([as][cpt]\)/%[\1]/g'`
    elif [ ".$opt_c" = .yes ]; then
        opt_F=`echo ":$opt_F" | sed -e 's/^://' -e 's/%\([as][cpt]\)/%<\1>/g'`
    fi

    #   provide verbose and concise variants
    AC_V=""; AC_N=""; AC_C=""
    AP_V=""; AP_N=""; AP_C=""
    AT_V=""; AT_N=""; AT_C=""
    SC_V=""; SC_N=""; SC_C=""
    SP_V=""; SP_N=""; SP_C=""
    ST_V=""; ST_N=""; ST_C=""
    for var_lc in at ap ac st sp sc; do
        case "$opt_F" in
            *"%[${val_lc}]"* | *"%{${val_lc}}"* | *"%${val_lc}"* | *"%<${val_lc}>"* )
            var_uc=`util_upper "$var_lc"`
            eval "val=\"\$${var_uc}\""
            val_V=""; val_N=""; val_C=""
            case "$opt_F" in
                *"%[${var_lc}]"* )
                    val_V=`echo ":$val" | \
                           sed -e 's/^://' \
                               -e 's;\[\([^]]*\)\];\1;g' \
                               -e 's;<\([^>]*\)>;\1;g' \
                               -e "s; ;§§;g" \
                               -e "s;/;%%;g" \
                               -e "s;§§;${opt_S};g" \
                               -e "s;%%;${opt_C};g"`
                    eval "${var_uc}_V=\"\${val_V}\""
                    ;;
            esac
            case "$opt_F" in
                *"%{${var_lc}}"* | *"%${var_lc}"* )
                    val_N=`echo ":$val" | \
                           sed -e 's/^://' \
                               -e 's;\[\([^]]*\)\];;g' \
                               -e 's;<\([^>]*\)>;\1;g' \
                               -e "s; ;§§;g" \
                               -e "s;/;%%;g" \
                               -e "s;§§;${opt_S};g" \
                               -e "s;%%;${opt_C};g"`
                    eval "${var_uc}_N=\"\${val_N}\""
                    ;;
            esac
            case "$opt_F" in
                *"%<${var_lc}>"* )
                    val_C=`echo ":$val" | \
                           sed -e 's/^://' \
                               -e 's;\[\([^]]*\)\];;g' \
                               -e 's;[^<]*<\([^>]*\)>[^<]*;\1;g' \
                               -e "s; ;§§;g" \
                               -e "s;/;%%;g" \
                               -e "s;§§;${opt_S};g" \
                               -e "s;%%;${opt_C};g"`
                    eval "${var_uc}_C=\"\${val_C}\""
                    ;;
            esac
            ;;
        esac
    done

    #   create output string
    output=`echo ":$opt_F" |\
            sed -e "s/^://" \
                -e "s;%\\[ac\\];${AC_V};g" \
                -e "s;%{ac};${AC_N};g" \
                -e "s;%ac;${AC_N};g" \
                -e "s;%<ac>;${AC_C};g" \
                -e "s;%\\[ap\\];${AP_V};g" \
                -e "s;%{ap};${AP_N};g" \
                -e "s;%ap;${AP_N};g" \
                -e "s;%<ap>;${AP_C};g" \
                -e "s;%\\[at\\];${AT_V};g" \
                -e "s;%{at};${AT_N};g" \
                -e "s;%at;${AT_N};g" \
                -e "s;%<at>;${AT_C};g" \
                -e "s;%\\[sc\\];${SC_V};g" \
                -e "s;%{sc};${SC_N};g" \
                -e "s;%sc;${SC_N};g" \
                -e "s;%<sc>;${SC_C};g" \
                -e "s;%\\[sp\\];${SP_V};g" \
                -e "s;%{sp};${SP_N};g" \
                -e "s;%sp;${SP_N};g" \
                -e "s;%<sp>;${SP_C};g" \
                -e "s;%\\[st\\];${ST_V};g" \
                -e "s;%{st};${ST_N};g" \
                -e "s;%st;${ST_N};g" \
                -e "s;%<st>;${ST_C};g" \
                -e 's/\\\\n/^/g' |\
             tr '^' '\012'`

    #   support lower/upper-case mapping
    if [ ".$opt_L" = .yes ]; then
        output=`util_lower "$output"`
    elif [ ".$opt_U" = .yes ]; then
        output=`util_upper "$output"`
    fi

    #   display output string
    if [ ".$opt_n" = .yes ]; then
        echo . | awk '{ printf("%s", output); }' output="$output"
    else
        echo "$output"
    fi

    shtool_exit 0
    ;;

path )
    ##
    ##  path -- Deal with program paths
    ##  Copyright (c) 1998-2008 Ralf S. Engelschall <rse@engelschall.com>
    ##

    namelist="$*"

    #   check whether the test command supports the -x option
    if [ -x /bin/sh ] 2>/dev/null; then
        minusx="-x"
    else
        minusx="-r"
    fi

    #   split path string
    paths="`echo $opt_p |\
            sed -e 's/^:/.:/' \
                -e 's/::/:.:/g' \
                -e 's/:$/:./' \
                -e 's/:/ /g'`"

    #   SPECIAL REQUEST
    #   translate forward to reverse path
    if [ ".$opt_r" = .yes ]; then
        if [ "x$namelist" = "x." ]; then
            rp='.'
        else
            rp=''
            for pe in `IFS="$IFS/"; echo $namelist`; do
                rp="../$rp"
            done
        fi
        echo $rp | sed -e 's:/$::'
        shtool_exit 0
    fi

    #   SPECIAL REQUEST
    #   strip out directory or base name
    if [ ".$opt_d" = .yes ]; then
        echo "$namelist" |\
        sed -e 's;[^/]*$;;' -e 's;\(.\)/$;\1;'
        shtool_exit 0
    fi
    if [ ".$opt_b" = .yes ]; then
        echo "$namelist" |\
        sed -e 's;.*/\([^/]*\)$;\1;'
        shtool_exit 0
    fi

    #   MAGIC SITUATION
    #   Perl Interpreter (perl)
    if [ ".$opt_m" = .yes ] && [ ".$namelist" = .perl ]; then
        rm -f $tmpfile >/dev/null 2>&1
        touch $tmpfile
        found=0
        pc=99
        for dir in $paths; do
            dir=`echo $dir | sed -e 's;/*$;;'`
            nc=99
            for name in perl perl5 miniperl; do
                 if [ $minusx "$dir/$name" ] && [ ! -d "$dir/$name" ]; then
                     perl="$dir/$name"
                     pv=`$perl -e 'printf("%.3f", $]);'`
                     echo "$pv:$pc:$nc:$perl" >>$tmpfile
                     found=1
                 fi
                 nc=`expr $nc - 1`
            done
            pc=`expr $pc - 1`
        done
        if [ $found = 1 ]; then
            perl="`cat $tmpfile | sort -r -u | sed -e 'q' | cut -d: -f4`"
            rm -f $tmpfile >/dev/null 2>&1
            echo "$perl"
            shtool_exit 0
        fi
        rm -f $tmpfile >/dev/null 2>&1
        shtool_exit 1
    fi

    #   MAGIC SITUATION
    #   C pre-processor (cpp)
    if [ ".$opt_m" = .yes ] && [ ".$namelist" = .cpp ]; then
        echo >$tmpfile.c "#include <assert.h>"
        echo >>$tmpfile.c "Syntax Error"
        #   1. try the standard cc -E approach
        cpp="${CC-cc} -E"
        (eval "$cpp $tmpfile.c >/dev/null") 2>$tmpfile.out
        my_error=`grep -v '^ *+' $tmpfile.out`
        if [ ".$my_error" != . ]; then
            #   2. try the cc -E approach and GCC's -traditional-ccp option
            cpp="${CC-cc} -E -traditional-cpp"
            (eval "$cpp $tmpfile.c >/dev/null") 2>$tmpfile.out
            my_error=`grep -v '^ *+' $tmpfile.out`
            if [ ".$my_error" != . ]; then
                #   3. try a standalone cpp command in path and lib dirs
                for path in $paths /lib /usr/lib /usr/local/lib; do
                    path=`echo $path | sed -e 's;/*$;;'`
                    if [ $minusx "$path/cpp" ] && [ ! -d "$path/cpp" ]; then
                        cpp="$path/cpp"
                        break
                    fi
                done
                if [ ".$cpp" != . ]; then
                    (eval "$cpp $tmpfile.c >/dev/null") 2>$tmpfile.out
                    my_error=`grep -v '^ *+' $tmpfile.out`
                    if [ ".$my_error" != . ]; then
                        #   ok, we gave up...
                        cpp=''
                    fi
                fi
            fi
        fi
        rm -f $tmpfile >/dev/null 2>&1
        rm -f $tmpfile.c $tmpfile.out >/dev/null 2>&1
        if [ ".$cpp" != . ]; then
            echo "$cpp"
            shtool_exit 0
        fi
        shtool_exit 1
    fi

    #   STANDARD SITUATION
    #   iterate over names
    for name in $namelist; do
        #   iterate over paths
        for path in $paths; do
            path=`echo $path | sed -e 's;/*$;;'`
            if [ $minusx "$path/$name" ] && [ ! -d "$path/$name" ]; then
                if [ ".$opt_s" != .yes ]; then
                    echo "$path/$name"
                fi
                shtool_exit 0
            fi
        done
    done

    shtool_exit 1
    ;;

esac

shtool_exit 0

