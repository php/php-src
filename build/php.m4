dnl This file contains local autoconf macros.

dnl ----------------------------------------------------------------------------
dnl Output stylize macros for configure (help/runtime).
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_HELP_SEPARATOR(title)
dnl
dnl Adds separator title into the configure --help display.
dnl
AC_DEFUN([PHP_HELP_SEPARATOR], [AC_ARG_ENABLE([], [
$1], [])])

dnl
dnl PHP_CONFIGURE_PART(title)
dnl
dnl Adds separator title configure output (idea borrowed from mm).
dnl
AC_DEFUN([PHP_CONFIGURE_PART],[
  AC_MSG_RESULT()
  AC_MSG_RESULT([${T_MD}$1${T_ME}])
])

dnl ----------------------------------------------------------------------------
dnl Build system helper macros.
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_RUN_ONCE(namespace, variable, code)
dnl
dnl Execute code, if variable is not set in namespace.
dnl
AC_DEFUN([PHP_RUN_ONCE],[
  changequote({,})
  unique=$(echo $2|$SED 's/[^a-zA-Z0-9]/_/g')
  changequote([,])
  cmd="echo $ac_n \"\$$1$unique$ac_c\""
  if test -n "$unique" && test "$(eval $cmd)" = "" ; then
    eval "$1$unique=set"
    $3
  fi
])

dnl
dnl PHP_EXPAND_PATH(path, variable)
dnl
dnl Expands path to an absolute path and assigns it to variable.
dnl
AC_DEFUN([PHP_EXPAND_PATH],[
  if test -z "$1" || echo "$1" | grep '^/' >/dev/null ; then
    $2=$1
  else
    changequote({,})
    ep_dir=$(echo $1|$SED 's%/*[^/][^/]*/*$%%')
    changequote([,])
    if test -z $ep_dir ; then
      ep_realdir=$(pwd)
    else
      ep_realdir=$(cd "$ep_dir" && pwd)
    fi
    $2="$ep_realdir"/$(basename "$1")
  fi
])

dnl
dnl PHP_SUBST(varname)
dnl
dnl Adds variable with its value into Makefile, e.g.:
dnl CC = gcc
dnl
AC_DEFUN([PHP_SUBST],[
  PHP_VAR_SUBST="$PHP_VAR_SUBST $1"
])

dnl
dnl PHP_SUBST_OLD(varname, [VALUE])
dnl
dnl Same as PHP_SUBST() but also substitutes all @VARNAME@ instances in every
dnl file passed to AC_OUTPUT.
dnl
AC_DEFUN([PHP_SUBST_OLD],[
  AC_SUBST($@)
  PHP_SUBST([$1])
])

dnl ----------------------------------------------------------------------------
dnl Build system base macros.
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_CANONICAL_HOST_TARGET
dnl
AC_DEFUN([PHP_CANONICAL_HOST_TARGET],[
  AC_REQUIRE([AC_CANONICAL_HOST])dnl
  AC_REQUIRE([AC_CANONICAL_TARGET])dnl
  dnl Make sure we do not continue if host_alias is empty.
  if test -z "$host_alias" && test -n "$host"; then
    host_alias=$host
  fi
  AS_VAR_IF([host_alias],,
    [AC_MSG_ERROR([host_alias is not set! Make sure to run config.guess])])
])

dnl
dnl PHP_INIT_BUILD_SYSTEM
dnl
dnl Initializes PHP build system configuration, creates build directories and
dnl adds Makefile placeholders.
dnl
AC_DEFUN([PHP_INIT_BUILD_SYSTEM],
[AC_REQUIRE([PHP_CANONICAL_HOST_TARGET])dnl
php_shtool=$srcdir/build/shtool
T_MD=$($php_shtool echo -n -e %B)
T_ME=$($php_shtool echo -n -e %b)

dnl Create empty Makefile placeholders.
> Makefile.objects
> Makefile.fragments

dnl Mark whether the CFLAGS are set to automatic default value by Autoconf, or
dnl they are manually modified by the environment variable from outside. E.g.
dnl './configure CFLAGS=...'. Set this before the AC_PROG_CC, where Autoconf
dnl adjusts the CFLAGS variable, so the checks can modify CFLAGS.
AS_VAR_IF([CFLAGS],, [auto_cflags=1])

dnl Required programs.
PHP_PROG_AWK

abs_srcdir=$(cd $srcdir && pwd)
abs_builddir=$(pwd)

dnl Run at the end of the configuration, before creating the config.status.
AC_CONFIG_COMMANDS_PRE(
[dnl Directory for storing shared objects of extensions.
PHP_ADD_BUILD_DIR([modules])
phplibdir="$(pwd)/modules"
PHP_SUBST([phplibdir])
PHP_GEN_BUILD_DIRS
PHP_GEN_GLOBAL_MAKEFILE
])dnl
])

dnl
dnl PHP_GEN_GLOBAL_MAKEFILE
dnl
dnl Generates the global makefile.
dnl
AC_DEFUN([PHP_GEN_GLOBAL_MAKEFILE],[
  AC_MSG_NOTICE([creating Makefile])
  cat >Makefile <<EOF
srcdir = $abs_srcdir
builddir = $abs_builddir
top_srcdir = $abs_srcdir
top_builddir = $abs_builddir
EOF
  for i in $PHP_VAR_SUBST; do
    eval echo "$i = \$$i" >> Makefile
  done

  cat $abs_srcdir/build/Makefile.global Makefile.fragments Makefile.objects >> Makefile
])

dnl
dnl PHP_ADD_MAKEFILE_FRAGMENT([makefile [, srcdir [, builddir]]])
dnl
dnl Processes a file called Makefile.frag in the source directory of the most
dnl recently added extension. $(srcdir) and $(builddir) are substituted with the
dnl proper paths. Can be used to supply custom rules and/or additional targets.
dnl For extensions, call this macro after the PHP_NEW_EXTENSION to get these
dnl variables substituted automatically, elsewhere pass the Makefile path
dnl "makefile" and optionally adjust "srcdir" and "builddir".
dnl
AC_DEFUN([PHP_ADD_MAKEFILE_FRAGMENT], [dnl
  ifelse($1,,src=$ext_srcdir/Makefile.frag,src=$1)
  ifelse($2,,ac_srcdir=$ext_srcdir,ac_srcdir=$2)
  ifelse($3,,ac_builddir=$ext_builddir,ac_builddir=$3)
  test -f "$src" && $SED -e "s#\$(srcdir)#$ac_srcdir#g" -e "s#\$(builddir)#$ac_builddir#g" $src  >> Makefile.fragments
])

dnl
dnl PHP_ADD_SOURCES(source-path, sources [, special-flags [, type]])
dnl
dnl Adds sources which are located relative to source-path to the array of type
dnl type. The blank-or-newline separated list of "sources" are processed with
dnl optional special-flags which are passed to the compiler. Sources can be
dnl either written in C or C++ (filenames shall end in .c or .cpp,
dnl respectively).
dnl
dnl Note: If source-path begins with a "/", the "/" is removed and the path is
dnl interpreted relative to the top build-directory.
dnl
dnl Which array to append to?
dnl
AC_DEFUN([PHP_ADD_SOURCES],[
  PHP_ADD_SOURCES_X($1, $2, $3, ifelse($4,sapi,PHP_SAPI_OBJS,PHP_GLOBAL_OBJS))
])

dnl
dnl _PHP_ASSIGN_BUILD_VARS(type)
dnl
dnl Internal, don't use.
dnl
AC_DEFUN([_PHP_ASSIGN_BUILD_VARS],[
ifelse($1,shared,[
  b_c_pre=$shared_c_pre
  b_cxx_pre=$shared_cxx_pre
  b_c_meta=$shared_c_meta
  b_cxx_meta=$shared_cxx_meta
  b_c_post=$shared_c_post
  b_cxx_post=$shared_cxx_post
],[
  b_c_pre=$php_c_pre
  b_cxx_pre=$php_cxx_pre
  b_c_meta=$php_c_meta
  b_cxx_meta=$php_cxx_meta
  b_c_post=$php_c_post
  b_cxx_post=$php_cxx_post
])dnl
  b_lo=[$]$1_lo
])

dnl
dnl PHP_ADD_SOURCES_X(source-path, sources[, special-flags[, target-var[, shared]]])
dnl
dnl Additional to PHP_ADD_SOURCES (see above), this lets you set the name of the
dnl array target-var directly, as well as whether shared objects will be built
dnl from the blank-or-newline-separated list of "sources". Should not be used
dnl directly.
dnl
AC_DEFUN([PHP_ADD_SOURCES_X],[
dnl Relative to source- or build-directory?
dnl ac_srcdir/ac_bdir include trailing slash
  case $1 in
  ""[)] ac_srcdir="$abs_srcdir/"; unset ac_bdir; ac_inc="-I. -I$abs_srcdir" ;;
  /*[)] ac_srcdir=$(echo "$1"|cut -c 2-)"/"; ac_bdir=$ac_srcdir; ac_inc="-I$ac_bdir -I$abs_srcdir/$ac_bdir" ;;
  *[)] ac_srcdir="$abs_srcdir/$1/"; ac_bdir="$1/"; ac_inc="-I$ac_bdir -I$ac_srcdir" ;;
  esac

dnl how to build .. shared or static?
  ifelse($5,yes,_PHP_ASSIGN_BUILD_VARS(shared),_PHP_ASSIGN_BUILD_VARS(php))

dnl Iterate over the sources.
  old_IFS=[$]IFS
  for ac_src in m4_normalize(m4_expand([$2])); do

dnl Remove the suffix.
      IFS=.
      set $ac_src
      ac_obj=[$]1
      IFS=$old_IFS

dnl Append to the array which has been dynamically chosen at m4 time.
      $4="[$]$4 [$]ac_bdir[$]ac_obj.lo"

dnl Choose the right compiler/flags/etc. for the source-file.
      case $ac_src in
        *.c[)] ac_comp="$b_c_pre $ac_inc $b_c_meta m4_normalize(m4_expand([$3])) -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $b_c_post" ;;
        *.s[)] ac_comp="$b_c_pre $ac_inc $b_c_meta m4_normalize(m4_expand([$3])) -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $b_c_post" ;;
        *.S[)] ac_comp="$b_c_pre $ac_inc $b_c_meta m4_normalize(m4_expand([$3])) -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $b_c_post" ;;
        *.cpp|*.cc|*.cxx[)] ac_comp="$b_cxx_pre $ac_inc $b_cxx_meta m4_normalize(m4_expand([$3])) -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $b_cxx_post" ;;
      esac

dnl Generate Makefiles with dependencies
      ac_comp="$ac_comp -MMD -MF $ac_bdir$ac_obj.dep -MT $ac_bdir[$]ac_obj.lo"

dnl Create a rule for the object/source combo.
    cat >>Makefile.objects<<EOF
-include $ac_bdir[$]ac_obj.dep
$ac_bdir[$]ac_obj.lo: $ac_srcdir[$]ac_src
	$ac_comp
EOF
  done
])

dnl ----------------------------------------------------------------------------
dnl Compiler characteristics checks.
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_RUNPATH_SWITCH
dnl
dnl Checks for -R, etc. switch.
dnl
AC_DEFUN([PHP_RUNPATH_SWITCH],[
AC_CACHE_CHECK([if compiler supports -Wl,-rpath,], [php_cv_cc_rpath], [
  SAVE_LIBS=$LIBS
  LIBS="-Wl,-rpath,/usr/$PHP_LIBDIR $LIBS"
  AC_LINK_IFELSE([AC_LANG_PROGRAM()],
    [php_cv_cc_rpath=yes],
    [php_cv_cc_rpath=no])
  LIBS=$SAVE_LIBS
])
AS_VAR_IF([php_cv_cc_rpath], [yes],
  [ld_runpath_switch=-Wl,-rpath,],
  [AC_CACHE_CHECK([if compiler supports -R], [php_cv_cc_dashr], [
    SAVE_LIBS=$LIBS
    LIBS="-R /usr/$PHP_LIBDIR $LIBS"
    AC_LINK_IFELSE([AC_LANG_PROGRAM()],
      [php_cv_cc_dashr=yes],
      [php_cv_cc_dashr=no])
    LIBS=$SAVE_LIBS
  ])
  AS_VAR_IF([php_cv_cc_dashr], [yes],
    [ld_runpath_switch=-R],
    [ld_runpath_switch=-L])
])
AS_VAR_IF([PHP_RPATH], [no], [ld_runpath_switch=])
])

dnl
dnl PHP_LIBGCC_LIBPATH(gcc)
dnl
dnl Stores the location of libgcc in libgcc_libpath.
dnl
AC_DEFUN([PHP_LIBGCC_LIBPATH],[
  changequote({,})
  libgcc_libpath=$($1 --print-libgcc-file-name|$SED 's%/*[^/][^/]*$%%')
  changequote([,])
])

dnl ----------------------------------------------------------------------------
dnl Macros to modify LIBS, INCLUDES, etc. variables.
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_REMOVE_USR_LIB(NAME)
dnl
dnl Removes all -L/usr/$PHP_LIBDIR entries from variable NAME.
dnl
AC_DEFUN([PHP_REMOVE_USR_LIB],[
  unset ac_new_flags
  for i in [$]$1; do
    case [$]i in
    -L/usr/$PHP_LIBDIR|-L/usr/$PHP_LIBDIR/[)] ;;
    *[)] ac_new_flags="[$]ac_new_flags [$]i" ;;
    esac
  done
  $1=[$]ac_new_flags
])

dnl
dnl PHP_EVAL_LIBLINE(libline, [libs-variable], [not-extension])
dnl
dnl Use this macro to add libraries and/or library search paths to the PHP build
dnl system when specified in compiler notation. Libraries (-l) are appended
dnl either to the global Autoconf LIBS variable or to the specified
dnl "libs-variable" (e.g., *_SHARED_LIBADD) when the extension is shared
dnl (ext_shared=yes). If "not-extension" is set to a non-blank value, the
dnl libraries are appended to "libs-variable" unconditionally (this is
dnl particularly useful when working with SAPIs). The -L flags are appended to
dnl the LDFLAGS.
dnl
AC_DEFUN([PHP_EVAL_LIBLINE],
[m4_ifnblank([$3], [m4_ifblank([$2],
  [m4_warn([syntax], [Missing 2nd argument when skipping extension check])],
  [_php_ext_shared_saved=$ext_shared; ext_shared=yes])])
  for ac_i in $1; do
    AS_CASE([$ac_i],
      [-pthread], [
        AS_VAR_IF([ext_shared], [yes], [$2="$$2 -pthread"], [
          PHP_RUN_ONCE([EXTRA_LDFLAGS], [$ac_i],
            [EXTRA_LDFLAGS="$EXTRA_LDFLAGS $ac_i"])
          PHP_RUN_ONCE([EXTRA_LDFLAGS_PROGRAM], [$ac_i],
            [EXTRA_LDFLAGS_PROGRAM="$EXTRA_LDFLAGS_PROGRAM $ac_i"])
        ])
      ],
      [-l*], [
        ac_ii=$(echo $ac_i|cut -c 3-)
        PHP_ADD_LIBRARY([$ac_ii], [yes], [$2])
      ],
      [-L*], [
        ac_ii=$(echo $ac_i|cut -c 3-)
        PHP_ADD_LIBPATH([$ac_ii], [$2])
      ])
  done
m4_ifnblank([$3], [m4_ifnblank([$2], [ext_shared=$_php_ext_shared_saved])])[]dnl
])

dnl
dnl PHP_EVAL_INCLINE(headerline)
dnl
dnl Use this macro, if you need to add header search paths to the PHP build
dnl system which are only given in compiler notation.
dnl
AC_DEFUN([PHP_EVAL_INCLINE],[
  for ac_i in $1; do
    case $ac_i in
    -I*[)]
      ac_ii=$(echo $ac_i|cut -c 3-)
      PHP_ADD_INCLUDE([$ac_ii])
    ;;
    esac
  done
])

dnl
dnl _PHP_ADD_LIBPATH_GLOBAL(variable)
dnl
dnl Internal, don't use.
dnl
AC_DEFUN([_PHP_ADD_LIBPATH_GLOBAL],[
  PHP_RUN_ONCE(LIBPATH, $1, [
    test -n "$ld_runpath_switch" && LDFLAGS="$LDFLAGS $ld_runpath_switch$1"
    LDFLAGS="$LDFLAGS -L$1"
    PHP_RPATHS="$PHP_RPATHS $1"
  ])
])

dnl
dnl PHP_ADD_LIBPATH(path [, SHARED-LIBADD])
dnl
dnl Adds a path to linkpath/runpath (LDFLAGS).
dnl
AC_DEFUN([PHP_ADD_LIBPATH],[
  if test "$1" != "/usr/$PHP_LIBDIR" && test "$1" != "/usr/lib"; then
    PHP_EXPAND_PATH($1, ai_p)
    ifelse([$2],,[
      _PHP_ADD_LIBPATH_GLOBAL([$ai_p])
    ],[
      if test "$ext_shared" = "yes"; then
        $2="-L$ai_p [$]$2"
        test -n "$ld_runpath_switch" && $2="$ld_runpath_switch$ai_p [$]$2"
      else
        _PHP_ADD_LIBPATH_GLOBAL([$ai_p])
      fi
    ])
  fi
])

dnl
dnl PHP_UTILIZE_RPATHS()
dnl
dnl Builds RPATHS/LDFLAGS from PHP_RPATHS.
dnl
AC_DEFUN([PHP_UTILIZE_RPATHS],[
  OLD_RPATHS=$PHP_RPATHS
  unset PHP_RPATHS

  for i in $OLD_RPATHS; do
    dnl Can be passed to native cc/libtool
    PHP_LDFLAGS="$PHP_LDFLAGS -L$i"
    dnl Libtool-specific
    PHP_RPATHS="$PHP_RPATHS -R $i"
    dnl cc-specific
    NATIVE_RPATHS="$NATIVE_RPATHS $ld_runpath_switch$i"
  done

  AS_VAR_IF([PHP_RPATH], [no], [
    unset PHP_RPATHS
    unset NATIVE_RPATHS
  ])
])

dnl
dnl PHP_ADD_INCLUDE(paths [,prepend])
dnl
dnl Add blank-or-newline-separated list of include paths. If "prepend" is given,
dnl paths are prepended to the beginning of INCLUDES.
dnl
AC_DEFUN([PHP_ADD_INCLUDE], [
for include_path in m4_normalize(m4_expand([$1])); do
  AS_IF([test "$include_path" != "/usr/include"], [
    PHP_EXPAND_PATH([$include_path], [ai_p])
    PHP_RUN_ONCE([INCLUDEPATH], [$ai_p], [m4_ifnblank([$2],
      [INCLUDES="-I$ai_p $INCLUDES"],
      [INCLUDES="$INCLUDES -I$ai_p"])])
  ])
done
])

dnl
dnl _PHP_X_ADD_LIBRARY
dnl
dnl Internal, don't use.
dnl
AC_DEFUN([_PHP_X_ADD_LIBRARY],[dnl
  ifelse([$2],,$3="-l$1 [$]$3", $3="[$]$3 -l$1") dnl
])

dnl
dnl _PHP_ADD_LIBRARY_SKELETON
dnl
dnl Internal, don't use.
dnl
AC_DEFUN([_PHP_ADD_LIBRARY_SKELETON],[
  case $1 in
  c|c_r[)] ;;
  *[)] ifelse($3,,[
    _PHP_X_ADD_LIBRARY($1,$2,$5)
  ],[
    if test "$ext_shared" = "yes"; then
      _PHP_X_ADD_LIBRARY($1,$2,$3)
    else
      $4($1,$2)
    fi
  ]) ;;
  esac
])

dnl
dnl PHP_ADD_LIBRARY(library[, append[, shared-libadd]])
dnl
dnl Add a library to the link line.
dnl
AC_DEFUN([PHP_ADD_LIBRARY],[
  _PHP_ADD_LIBRARY_SKELETON([$1],[$2],[$3],[PHP_ADD_LIBRARY],[LIBS])
])

dnl
dnl PHP_ADD_LIBRARY_DEFER(library[, append[, shared-libadd]])
dnl
dnl Add a library to the link line (deferred, not used during configure).
dnl
AC_DEFUN([PHP_ADD_LIBRARY_DEFER],[
  _PHP_ADD_LIBRARY_SKELETON([$1],[$2],[$3],[PHP_ADD_LIBRARY_DEFER],[DLIBS])
])

dnl
dnl PHP_ADD_LIBRARY_WITH_PATH(library, path[, shared-libadd])
dnl
dnl Add a library to the link line and path to linkpath/runpath. If
dnl shared-libadd is not empty and $ext_shared is yes, shared-libadd will be
dnl assigned the library information.
dnl
AC_DEFUN([PHP_ADD_LIBRARY_WITH_PATH],[
ifelse($3,,[
  if test -n "$2"; then
    PHP_ADD_LIBPATH($2)
  fi
  PHP_ADD_LIBRARY($1)
],[
  if test "$ext_shared" = "yes"; then
    $3="-l$1 [$]$3"
    if test -n "$2"; then
      PHP_ADD_LIBPATH($2,$3)
    fi
  else
    PHP_ADD_LIBRARY_WITH_PATH($1,$2)
  fi
])
])

dnl
dnl PHP_ADD_LIBRARY_DEFER_WITH_PATH(library, path[, shared-libadd])
dnl
dnl Add a library to the link line (deferred) and path to linkpath/runpath (not
dnl deferred). If shared-libadd is not empty and $ext_shared is yes,
dnl shared-libadd will be assigned the library information.
dnl
AC_DEFUN([PHP_ADD_LIBRARY_DEFER_WITH_PATH],[
ifelse($3,,[
  if test -n "$2"; then
    PHP_ADD_LIBPATH($2)
  fi
  PHP_ADD_LIBRARY_DEFER($1)
],[
  if test "$ext_shared" = "yes"; then
    $3="-l$1 [$]$3"
    if test -n "$2"; then
      PHP_ADD_LIBPATH($2,$3)
    fi
  else
    PHP_ADD_LIBRARY_DEFER_WITH_PATH($1,$2)
  fi
])
])

dnl
dnl PHP_ADD_FRAMEWORK(framework [,before])
dnl
dnl Add a (Darwin / Mac OS X) framework to the link line. If before is 1, the
dnl framework is added to the beginning of the line.
dnl
AC_DEFUN([PHP_ADD_FRAMEWORK], [
  PHP_RUN_ONCE(FRAMEWORKS, $1, [
    if test "$2"; then
      PHP_FRAMEWORKS="-framework $1 $PHP_FRAMEWORKS"
    else
      PHP_FRAMEWORKS="$PHP_FRAMEWORKS -framework $1"
    fi
  ])
])

dnl
dnl PHP_ADD_FRAMEWORKPATH(path [,before])
dnl
dnl Add a (Darwin / Mac OS X) framework path to the link and include lines.
dnl Default paths include (but are not limited to) /Local/Library/Frameworks and
dnl /System/Library/Frameworks, so these don't need to be specifically added. If
dnl before is 1, the framework path is added to the beginning of the relevant
dnl lines.
dnl
AC_DEFUN([PHP_ADD_FRAMEWORKPATH], [
  PHP_EXPAND_PATH($1, ai_p)
  PHP_RUN_ONCE(FRAMEWORKPATH, $ai_p, [
    if test "$2"; then
      PHP_FRAMEWORKPATH="-F$ai_p $PHP_FRAMEWORKPATH"
    else
      PHP_FRAMEWORKPATH="$PHP_FRAMEWORKPATH -F$ai_p"
    fi
  ])
])

dnl
dnl PHP_ADD_FRAMEWORK_WITH_PATH(framework, path)
dnl
dnl Adds a (Darwin / Mac OS X) framework path and the framework itself to the
dnl link and include lines.
dnl
AC_DEFUN([PHP_ADD_FRAMEWORK_WITH_PATH], [
  PHP_ADD_FRAMEWORKPATH($2)
  PHP_ADD_FRAMEWORK($1)
])

dnl
dnl PHP_SET_LIBTOOL_VARIABLE(var)
dnl
dnl Set libtool variable.
dnl
AC_DEFUN([PHP_SET_LIBTOOL_VARIABLE], [
AS_VAR_IF([LIBTOOL],,
  [LIBTOOL='$(SHELL) $(top_builddir)/libtool $1'],
  [LIBTOOL="$LIBTOOL $1"])
])

dnl ----------------------------------------------------------------------------
dnl Wrapper macros for AC_ARG_WITH / AC_ARG_ENABLE
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_ARG_ANALYZE_EX
dnl
dnl Internal.
dnl
AC_DEFUN([PHP_ARG_ANALYZE_EX],[
ext_output="yes, shared"
ext_shared=yes
case [$]$1 in
shared,*[)]
  $1=$(echo "[$]$1"|$SED 's/^shared,//')
  ;;
shared[)]
  $1=yes
  ;;
no[)]
  ext_output=no
  ext_shared=no
  ;;
*[)]
  ext_output=yes
  ext_shared=no
  ;;
esac

dnl When using phpize, automatically enable and build extension as shared.
m4_ifdef([PHP_ALWAYS_SHARED], [PHP_ALWAYS_SHARED([$1])])
])

dnl
dnl PHP_ARG_ANALYZE
dnl
dnl Internal.
dnl
AC_DEFUN([PHP_ARG_ANALYZE],[
ifelse([$3],yes,[PHP_ARG_ANALYZE_EX([$1])],[ext_output=ifelse([$]$1,,no,[$]$1)])
ifelse([$2],,,[AC_MSG_RESULT([$ext_output])])
])

dnl
dnl PHP_ARG_WITH(arg-name, check message, help text[, default-val[, extension-or-not]])
dnl
dnl Sets PHP_ARG_NAME either to the user value or to the default value.
dnl default-val defaults to no. This will also set the variable ext_shared, and
dnl will overwrite any previous variable of that name. If extension-or-not is
dnl yes (default), then do the ENABLE_ALL check and run the PHP_ARG_ANALYZE_EX.
dnl
AC_DEFUN([PHP_ARG_WITH],[
php_with_[]translit($1,A-Z0-9-,a-z0-9_)=ifelse($4,,no,$4)
PHP_REAL_ARG_WITH([$1],[$2],[$3],[$4],PHP_[]translit($1,a-z0-9-,A-Z0-9_),[ifelse($5,,yes,$5)])
])

dnl
dnl PHP_REAL_ARG_WITH
dnl
dnl Internal.
dnl
AC_DEFUN([PHP_REAL_ARG_WITH],[
ifelse([$2],,,[AC_MSG_CHECKING([$2])])
AC_ARG_WITH($1,[$3],$5=[$]withval,
[
  $5=ifelse($4,,no,$4)
  ifelse($6,yes,[test "$PHP_ENABLE_ALL" && $5=$PHP_ENABLE_ALL])
])
PHP_ARG_ANALYZE($5,[$2],$6)
])

dnl
dnl PHP_ARG_ENABLE(arg-name, check message, help text[, default-val[, extension-or-not]])
dnl
dnl Sets PHP_ARG_NAME either to the user value or to the default value.
dnl default-val defaults to no. This will also set the variable ext_shared, and
dnl will overwrite any previous variable of that name. If extension-or-not is
dnl yes (default), then do the ENABLE_ALL check and run the PHP_ARG_ANALYZE_EX.
dnl
AC_DEFUN([PHP_ARG_ENABLE],[
php_enable_[]translit($1,A-Z0-9-,a-z0-9_)=ifelse($4,,no,$4)
PHP_REAL_ARG_ENABLE([$1],[$2],[$3],[$4],PHP_[]translit($1,a-z0-9-,A-Z0-9_),[ifelse($5,,yes,$5)])
])

dnl
dnl PHP_REAL_ARG_ENABLE
dnl
dnl Internal.
dnl
AC_DEFUN([PHP_REAL_ARG_ENABLE],[
ifelse([$2],,,[AC_MSG_CHECKING([$2])])
AC_ARG_ENABLE($1,[$3],$5=[$]enableval,
[
  $5=ifelse($4,,no,$4)
  ifelse($6,yes,[test "$PHP_ENABLE_ALL" && $5=$PHP_ENABLE_ALL])
])
PHP_ARG_ANALYZE($5,[$2],$6)
])

dnl ----------------------------------------------------------------------------
dnl Build macros
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_REQUIRE_CXX
dnl
AC_DEFUN([PHP_REQUIRE_CXX], [
AS_VAR_IF([php_cxx_done],, [
  AC_PROG_CXX
  AC_PROG_CXXCPP
  PHP_ADD_LIBRARY([stdc++])
  php_cxx_done=yes
])
])

dnl
dnl PHP_BUILD_SHARED
dnl
AC_DEFUN([PHP_BUILD_SHARED],[
  PHP_BUILD_PROGRAM
  OVERALL_TARGET=libphp.la
  php_sapi_module=shared

  php_c_pre=$shared_c_pre
  php_c_meta=$shared_c_meta
  php_c_post=$shared_c_post
  php_cxx_pre=$shared_cxx_pre
  php_cxx_meta=$shared_cxx_meta
  php_cxx_post=$shared_cxx_post
  php_lo=$shared_lo
])

dnl
dnl PHP_BUILD_SHARED_DYLIB
dnl
AC_DEFUN([PHP_BUILD_SHARED_DYLIB],[
  PHP_BUILD_SHARED
  OVERALL_TARGET=libphp.dylib
])

dnl
dnl PHP_BUILD_STATIC
dnl
AC_DEFUN([PHP_BUILD_STATIC],[
  PHP_BUILD_PROGRAM
  OVERALL_TARGET=libphp.la
  php_sapi_module=static
])

dnl
dnl PHP_BUILD_BUNDLE
dnl
AC_DEFUN([PHP_BUILD_BUNDLE],[
  PHP_BUILD_PROGRAM
  OVERALL_TARGET=libs/libphp.bundle
  php_sapi_module=static
])

dnl
dnl PHP_BUILD_PROGRAM
dnl
AC_DEFUN([PHP_BUILD_PROGRAM],[
  php_c_pre='$(LIBTOOL) --tag=CC --mode=compile $(CC)'
  php_c_meta='$(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)'
  php_c_post=
  php_cxx_pre='$(LIBTOOL) --tag=CXX --mode=compile $(CXX)'
  php_cxx_meta='$(COMMON_FLAGS) $(CXXFLAGS_CLEAN) $(EXTRA_CXXFLAGS)'
  php_cxx_post=
  php_lo=lo

  case $with_pic in
    yes) pic_setting='-prefer-pic';;
    no)  pic_setting='-prefer-non-pic';;
  esac

  shared_c_pre='$(LIBTOOL) --tag=CC --mode=compile $(CC)'
  shared_c_meta='$(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) '$pic_setting
  shared_c_post=
  shared_cxx_pre='$(LIBTOOL) --tag=CXX --mode=compile $(CXX)'
  shared_cxx_meta='$(COMMON_FLAGS) $(CXXFLAGS_CLEAN) $(EXTRA_CXXFLAGS) '$pic_setting
  shared_cxx_post=
  shared_lo=lo
])

dnl
dnl PHP_SHARED_MODULE(module-name, object-var, build-dir, cxx, zend_ext)
dnl
dnl Basically sets up the link-stage for building module-name from object_var in
dnl build-dir.
dnl
AC_DEFUN([PHP_SHARED_MODULE],[
  install_modules="install-modules"
  suffix=la

  AS_CASE([$host_alias], [*aix*], [additional_flags="-Wl,-G"])

  if test "x$5" = "xyes"; then
    PHP_ZEND_EX="$PHP_ZEND_EX \$(phplibdir)/$1.$suffix"
  else
    PHP_MODULES="$PHP_MODULES \$(phplibdir)/$1.$suffix"
  fi
  PHP_SUBST([$2])
  cat >>Makefile.objects<<EOF
\$(phplibdir)/$1.$suffix: $3/$1.$suffix
	\$(LIBTOOL) --tag=ifelse($4,,CC,CXX) --mode=install cp $3/$1.$suffix \$(phplibdir)

$3/$1.$suffix: \$($2) \$(translit($1,a-z_-,A-Z__)_SHARED_DEPENDENCIES)
	\$(LIBTOOL) --tag=ifelse($4,,CC,CXX) --mode=link ifelse($4,,[\$(CC)],[\$(CXX)]) -shared \$(COMMON_FLAGS) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(LDFLAGS) $additional_flags -o [\$]@ -export-dynamic -avoid-version -prefer-pic -module -rpath \$(phplibdir) \$(EXTRA_LDFLAGS) \$($2) \$(translit($1,a-z_-,A-Z__)_SHARED_LIBADD)

EOF
])

dnl
dnl PHP_SELECT_SAPI(name, type[, sources [, extra-cflags]])
dnl
dnl When developing PHP SAPI modules, this macro specifies the SAPI "name" by
dnl its "type", how PHP is supposed to be built (static, shared, bundle, or
dnl program). It optionally adds the blank-or-newline-separated source files
dnl "sources" and compilation flags "extra-cflags" to build the SAPI-specific
dnl objects. For example:
dnl   PHP_SELECT_SAPI([apache2handler],
dnl     [shared],
dnl     [<sources...>],
dnl     [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
dnl
AC_DEFUN([PHP_SELECT_SAPI],[
  if test "$2" = "program"; then
    PHP_BINARIES="$PHP_BINARIES $1"
  elif test "$PHP_SAPI" != "none"; then
    AC_MSG_ERROR([
+--------------------------------------------------------------------+
|                        *** ATTENTION ***                           |
|                                                                    |
| You've configured multiple SAPIs to be built. You can build only   |
| one SAPI module plus CGI, CLI and FPM binaries at the same time.   |
+--------------------------------------------------------------------+
])
  else
    PHP_SAPI=$1
  fi

  PHP_ADD_BUILD_DIR([sapi/$1])

  PHP_INSTALLED_SAPIS="$PHP_INSTALLED_SAPIS $1"

  ifelse($2,program,[
    PHP_BUILD_PROGRAM
    install_binaries="install-binaries"
    install_binary_targets="$install_binary_targets install-$1"
    PHP_SUBST(PHP_[]translit($1,a-z0-9-,A-Z0-9_)[]_OBJS)
    ifelse($3,,,[PHP_ADD_SOURCES_X([sapi/$1],[$3],[$4],PHP_[]translit($1,a-z0-9-,A-Z0-9_)[]_OBJS)])
  ],[
    case "$2" in
    static[)] PHP_BUILD_STATIC;;
    shared[)] PHP_BUILD_SHARED;;
    shared-dylib[)] PHP_BUILD_SHARED_DYLIB;;
    bundle[)] PHP_BUILD_BUNDLE;;
    esac
    install_sapi="install-sapi"
    ifelse($3,,,[PHP_ADD_SOURCES([sapi/$1],[$3],[$4],[sapi])])
  ])
])

dnl
dnl PHP_ADD_BUILD_DIR(dirs [, create])
dnl
dnl Add blank-or-newline-separated list of build directories or directories
dnl required for the out-of-source builds. When "create" is given, the provided
dnl "dirs" are created immediately upon macro invocation, instead of deferring
dnl them to the PHP_GEN_BUILD_DIRS.
dnl
AC_DEFUN([PHP_ADD_BUILD_DIR],
[m4_ifblank([$2],
  [AS_VAR_APPEND([BUILD_DIR], [" m4_normalize(m4_expand([$1]))"])],
  [$php_shtool mkdir -p m4_normalize(m4_expand([$1]))])
])

dnl
dnl PHP_GEN_BUILD_DIRS
dnl
AC_DEFUN([PHP_GEN_BUILD_DIRS],[
  AC_MSG_NOTICE([creating build directories])
  $php_shtool mkdir -p $BUILD_DIR
])

dnl
dnl PHP_NEW_EXTENSION(extname, sources [, shared [, sapi_class [, extra-cflags [, cxx [, zend_ext]]]]])
dnl
dnl Includes an extension in the build.
dnl
dnl "extname" is the name of the extension.
dnl "sources" is a blank-or-newline-separated list of source files relative to
dnl the subdir which are used to build the extension.
dnl "shared" can be set to "shared" or "yes" to build the extension as a
dnl dynamically loadable library. Optional parameter "sapi_class" can be set to
dnl "cli" to mark extension build only with CLI or CGI sapi's. "extra-cflags"
dnl are passed to the compiler, with @ext_srcdir@ and @ext_builddir@ being
dnl substituted.
dnl "cxx" can be used to indicate that a C++ shared module is desired.
dnl "zend_ext" indicates a zend extension.
AC_DEFUN([PHP_NEW_EXTENSION],[
  ext_builddir=[]PHP_EXT_BUILDDIR()
  ext_srcdir=[]PHP_EXT_SRCDIR()
  ext_dir=[]PHP_EXT_DIR()

  ifelse($5,,ac_extra=,[ac_extra=$(echo "m4_normalize(m4_expand([$5]))"|$SED s#@ext_srcdir@#$ext_srcdir#g|$SED s#@ext_builddir@#$ext_builddir#g)])

  if test "$3" != "shared" && test "$3" != "yes" && test "$4" != "cli"; then
dnl ---------------------------------------------- Static module
    [PHP_]translit($1,a-z_-,A-Z__)[_SHARED]=no
    PHP_ADD_SOURCES($ext_dir,$2,$ac_extra,)
    EXT_STATIC="$EXT_STATIC $1;$ext_dir"
    if test "$3" != "nocli"; then
      EXT_CLI_STATIC="$EXT_CLI_STATIC $1;$ext_dir"
    fi
  else
    if test "$3" = "shared" || test "$3" = "yes"; then
dnl ---------------------------------------------- Shared module
      [PHP_]translit($1,a-z_-,A-Z__)[_SHARED]=yes
      PHP_ADD_SOURCES_X($ext_dir,$2,$ac_extra -DZEND_COMPILE_DL_EXT=1,shared_objects_$1,yes)
      PHP_SHARED_MODULE($1,shared_objects_$1, $ext_builddir, $6, $7)
      AC_DEFINE_UNQUOTED([COMPILE_DL_]translit($1,a-z_-,A-Z__), [1],
        [Define to 1 if the PHP extension '$1' is built as a dynamic module.])
    fi
  fi

  if test "$3" != "shared" && test "$3" != "yes" && test "$4" = "cli"; then
dnl ---------------------------------------------- CLI static module
    [PHP_]translit($1,a-z_-,A-Z__)[_SHARED]=no
    case "$PHP_SAPI" in
      cgi|embed|phpdbg[)]
        PHP_ADD_SOURCES($ext_dir,$2,$ac_extra,)
        EXT_STATIC="$EXT_STATIC $1;$ext_dir"
        ;;
      *[)]
        PHP_ADD_SOURCES($ext_dir,$2,$ac_extra,cli)
        ;;
    esac
    EXT_CLI_STATIC="$EXT_CLI_STATIC $1;$ext_dir"
  fi
  PHP_ADD_BUILD_DIR([$ext_builddir])

dnl Set for phpize builds only.
dnl ---------------------------
  if test "$ext_builddir" = "."; then
    PHP_PECL_EXTENSION=$1
    PHP_SUBST([PHP_PECL_EXTENSION])
  fi
])

dnl
dnl PHP_ADD_EXTENSION_DEP(extname, depends [, depconf])
dnl
dnl This macro is scanned by genif.sh when it builds the internal functions
dnl list, so that modules can be init'd in the correct order
dnl $1 = name of extension, $2 = extension upon which it depends
dnl $3 = optional: if true, it's ok for $2 to have not been configured default
dnl is false and should halt the build. To be effective, this macro must be
dnl invoked *after* PHP_NEW_EXTENSION. The extension on which it depends must
dnl also have been configured. Due to the limited genif.sh parsing and regex
dnl matching implementation, this macro must be called on its own line, and its
dnl arguments must be passed unquoted (without Autoconf '[' and ']' characters.
dnl For Windows, see 'ADD_EXTENSION_DEP' in the win32 build.
dnl
AC_DEFUN([PHP_ADD_EXTENSION_DEP], [
  am_i_shared=$[PHP_]translit($1,a-z_-,A-Z__)[_SHARED]
  is_it_shared=$[PHP_]translit($2,a-z_-,A-Z__)[_SHARED]
  is_it_enabled=$[PHP_]translit($2,a-z_-,A-Z__)
  if test "$am_i_shared" = "no" && test "$is_it_shared" = "yes" ; then
    AC_MSG_ERROR([
You've configured extension $1 to build statically, but it
depends on extension $2, which you've configured to build shared.
You either need to build $1 shared or build $2 statically for the
build to be successful.
])
  fi
  if test "x$is_it_enabled" = "xno" && test "x$3" != "xtrue"; then
    AC_MSG_ERROR([
You've configured extension $1, which depends on extension $2,
but you've either not enabled $2, or have disabled it.
])
  fi
  dnl Some systems require that we link $2 to $1 when building.
])

dnl ----------------------------------------------------------------------------
dnl Checks for structures, typedefs, broken functions, etc.
dnl ----------------------------------------------------------------------------

dnl
dnl _PHP_CHECK_SIZEOF(type, cross-value, extra-headers [, found-action [, not-found-action]])
dnl
dnl Internal helper macro.
dnl
AC_DEFUN([_PHP_CHECK_SIZEOF],
[m4_warn([obsolete],
  [The PHP_CHECK_SIZEOF macro is obsolete. Use AC_CHECK_SIZEOF.])
  php_cache_value=php_cv_sizeof_[]$1
  AC_CACHE_VAL(php_cv_sizeof_[]$1, [
    old_LIBS=$LIBS
    LIBS=
    old_LDFLAGS=$LDFLAGS
    LDFLAGS=
    AC_RUN_IFELSE([AC_LANG_SOURCE([#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
$3

int main(void)
{
  FILE *fp = fopen("conftestval", "w");
  if (!fp) return(1);
  fprintf(fp, "%d\n", sizeof($1));
  return(0);
}
  ])], [
    eval $php_cache_value=$(cat conftestval)
  ], [
    eval $php_cache_value=0
  ], [
    ifelse([$2],,[eval $php_cache_value=0], [eval $php_cache_value=$2])
])
  LDFLAGS=$old_LDFLAGS
  LIBS=$old_LIBS
])
  if eval test "\$$php_cache_value" != "0"; then
ifelse([$4],[],:,[$4])
ifelse([$5],[],,[else $5])
  fi
])

dnl
dnl PHP_CHECK_SIZEOF(type, cross-value, extra-headers)
dnl
dnl Checks the size of specified "type". This macro is obsolete as of PHP 8.5 in
dnl favor of the AC_CHECK_SIZEOF.
dnl
AC_DEFUN([PHP_CHECK_SIZEOF], [
  AC_MSG_CHECKING([size of $1])
  _PHP_CHECK_SIZEOF($1, $2, $3, [
    AC_DEFINE_UNQUOTED([SIZEOF_]translit($1,a-z,A-Z_), [$]php_cv_sizeof_[]$1,
      [The size of '$1', as computed by sizeof.])
    AC_DEFINE_UNQUOTED([HAVE_]translit($1,a-z,A-Z_), [1],
      [Define to 1 if the system has the type '$1'.])
  ])
  AC_MSG_RESULT([[$][php_cv_sizeof_]translit($1, ,_)])
])

dnl
dnl PHP_TIME_R_TYPE
dnl
dnl Check type of reentrant time-related functions. Type can be: irix, hpux or
dnl POSIX.
dnl
AC_DEFUN([PHP_TIME_R_TYPE],
[AC_CACHE_CHECK([for type of reentrant time-related functions],
  [php_cv_time_r_type],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <time.h>

int main(void) {
char buf[27];
struct tm t;
time_t old = 0;
int r, s;

s = gmtime_r(&old, &t);
r = (int) asctime_r(&t, buf, 26);
if (r == s && s == 0) return (0);
return (1);
}
  ]])],
  [php_cv_time_r_type=hpux],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <time.h>
int main(void) {
  struct tm t, *s;
  time_t old = 0;
  char buf[27], *p;

  s = gmtime_r(&old, &t);
  p = asctime_r(&t, buf, 26);
  if (p == buf && s == &t) return (0);
  return (1);
}
  ]])],
  [php_cv_time_r_type=irix],
  [php_cv_time_r_type=POSIX],
  [php_cv_time_r_type=POSIX])],
  [php_cv_time_r_type=POSIX])
])
AS_CASE([$php_cv_time_r_type],
  [hpux], [AC_DEFINE([PHP_HPUX_TIME_R], [1],
    [Define to 1 if you have HP-UX 10.x.-style reentrant time functions.])],
  [irix], [AC_DEFINE([PHP_IRIX_TIME_R], [1],
    [Define to 1 you have IRIX-style reentrant time functions.])])
])

dnl
dnl PHP_DOES_PWRITE_WORK
dnl
dnl Internal.
dnl
AC_DEFUN([PHP_DOES_PWRITE_WORK], [
AC_RUN_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
$1
    int main(void) {
    int fd = open("conftest_pwrite", O_WRONLY|O_CREAT, 0600);

    if (fd < 0) return 1;
    if (pwrite(fd, "text", 4, 0) != 4) return 1;
    /* Linux glibc breakage until 2.2.5 */
    if (pwrite(fd, "text", 4, -1) != -1 || errno != EINVAL) return 1;
    return 0;
    }
  ])],
  [php_cv_func_pwrite=yes],
  [php_cv_func_pwrite=no],
  [php_cv_func_pwrite=no])
])

dnl
dnl PHP_DOES_PREAD_WORK
dnl
dnl Internal.
dnl
AC_DEFUN([PHP_DOES_PREAD_WORK], [
echo test > conftest_pread
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
$1
    int main(void) {
    char buf[3];
    int fd = open("conftest_pread", O_RDONLY);
    if (fd < 0) return 1;
    if (pread(fd, buf, 2, 0) != 2) return 1;
    /* Linux glibc breakage until 2.2.5 */
    if (pread(fd, buf, 2, -1) != -1 || errno != EINVAL) return 1;
    return 0;
    }
  ]])],
  [php_cv_func_pread=yes],
  [php_cv_func_pread=no],
  [php_cv_func_pread=no])
])

dnl
dnl PHP_PWRITE_TEST
dnl
AC_DEFUN([PHP_PWRITE_TEST], [
AC_CACHE_CHECK([whether pwrite works], [php_cv_func_pwrite], [
  PHP_DOES_PWRITE_WORK
  AS_VAR_IF([php_cv_func_pwrite], [no], [
    PHP_DOES_PWRITE_WORK([ssize_t pwrite(int, void *, size_t, off64_t);])
    AS_VAR_IF([php_cv_func_pwrite], [yes], [php_cv_func_pwrite=64])
  ])
])

AS_VAR_IF([php_cv_func_pwrite], [no],, [
  AC_DEFINE([HAVE_PWRITE], [1],
    [Define to 1 if you have the 'pwrite' function.])
  AS_VAR_IF([php_cv_func_pwrite], [64],
    [AC_DEFINE([PHP_PWRITE_64], [1],
      [Define to 1 if 'pwrite' declaration with 'off64_t' is missing.])])
])
])

dnl
dnl PHP_PREAD_TEST
dnl
AC_DEFUN([PHP_PREAD_TEST], [
AC_CACHE_CHECK([whether pread works], [php_cv_func_pread], [
  PHP_DOES_PREAD_WORK
  AS_VAR_IF([php_cv_func_pread], [no], [
    PHP_DOES_PREAD_WORK([ssize_t pread(int, void *, size_t, off64_t);])
    AS_VAR_IF([php_cv_func_pread], [yes], [php_cv_func_pread=64])
  ])
])

AS_VAR_IF([php_cv_func_pread], [no],, [
  AC_DEFINE([HAVE_PREAD], [1],
    [Define to 1 if you have the 'pread' function.])
  AS_VAR_IF([php_cv_func_pread], [64],
    [AC_DEFINE([PHP_PREAD_64], [1],
      [Define to 1 if 'pread' declaration with 'off64_t' is missing.])])
])
])

dnl
dnl PHP_MISSING_TIME_R_DECL
dnl
AC_DEFUN([PHP_MISSING_TIME_R_DECL],[
AC_CHECK_DECL([localtime_r],,
  [AC_DEFINE([MISSING_LOCALTIME_R_DECL], [1],
    [Define to 1 if 'localtime_r' declaration is missing.])],
  [#include <time.h>])
AC_CHECK_DECL([gmtime_r],,
  [AC_DEFINE([MISSING_GMTIME_R_DECL], [1],
    [Define to 1 if 'gmtime_r' declaration is missing.])],
  [#include <time.h>])
AC_CHECK_DECL([asctime_r],,
  [AC_DEFINE([MISSING_ASCTIME_R_DECL], [1],
    [Define to 1 if 'asctime_r' declaration is missing.])],
  [#include <time.h>])
AC_CHECK_DECL([ctime_r],,
  [AC_DEFINE([MISSING_CTIME_R_DECL], [1],
    [Define to 1 if 'ctime_r' declaration is missing.])],
  [#include <time.h>])
AC_CHECK_DECL([strtok_r],,
  [AC_DEFINE([MISSING_STRTOK_R_DECL], [1],
    [Define to 1 if 'strtok_r' declaration is missing.])],
  [#include <string.h>])
])

dnl
dnl PHP_EBCDIC
dnl
AC_DEFUN([PHP_EBCDIC],
[AC_CACHE_CHECK([whether system uses EBCDIC], [ac_cv_ebcdic],
[AC_RUN_IFELSE([AC_LANG_SOURCE([
int main(void) {
  return (unsigned char)'A' != (unsigned char)0xC1;
}
])],
  [ac_cv_ebcdic=yes],
  [ac_cv_ebcdic=no],
  [ac_cv_ebcdic=no])])
AS_VAR_IF([ac_cv_ebcdic], [yes],
  [AC_MSG_FAILURE([PHP does not support EBCDIC targets.])])
])

dnl
dnl PHP_BROKEN_GETCWD
dnl
dnl Some systems, notably Solaris, cause getcwd() or realpath to fail if a
dnl component of the path has execute but not read permissions.
dnl
AC_DEFUN([PHP_BROKEN_GETCWD], [
AC_CACHE_CHECK([for broken getcwd], [php_cv_func_getcwd_broken],
  [AS_CASE([$host_alias],
    [*solaris*], [php_cv_func_getcwd_broken=yes],
    [php_cv_func_getcwd_broken=no])])
AS_VAR_IF([php_cv_func_getcwd_broken], [yes],
  [AC_DEFINE([HAVE_BROKEN_GETCWD], [1],
    [Define to 1 if system has a broken 'getcwd'.])])
])

dnl
dnl PHP_BROKEN_GCC_STRLEN_OPT
dnl
dnl Early releases of GCC 8 shipped with a strlen() optimization bug, so they
dnl didn't properly handle the 'char val[1]' struct hack. See bug #76510.
dnl
AC_DEFUN([PHP_BROKEN_GCC_STRLEN_OPT],
[AC_CACHE_CHECK([for broken gcc optimize-strlen],
  [php_cv_have_broken_gcc_strlen_opt],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
struct s
{
  int i;
  char c[1];
};
int main(void)
{
  struct s *s = malloc(sizeof(struct s) + 3);
  s->i = 3;
  strcpy(s->c, "foo");
  return strlen(s->c+1) == 2;
}
]])],
  [php_cv_have_broken_gcc_strlen_opt=yes],
  [php_cv_have_broken_gcc_strlen_opt=no],
  [php_cv_have_broken_gcc_strlen_opt=no])])
AS_VAR_IF([php_cv_have_broken_gcc_strlen_opt], [yes],
  [AS_VAR_APPEND([CFLAGS], [" -fno-optimize-strlen"])])
])

dnl
dnl PHP_FOPENCOOKIE
dnl
dnl Check for cookie_io_functions_t type and fopencookie(). When using glibc,
dnl checks here require _GNU_SOURCE defined via AC_USE_SYSTEM_EXTENSIONS. Some
dnl glibc versions started using the off64_t in fopencookie seeker definition,
dnl which is in transition to use the POSIX standard off_t on 32-bit (with large
dnl file support enabled) and 64-bit.
dnl
AC_DEFUN([PHP_FOPENCOOKIE],
[AC_CHECK_TYPE([cookie_io_functions_t], [AC_CHECK_FUNCS([fopencookie])],,
  [#include <stdio.h>])
dnl Check if glibc might use off64_t seeker definition.
AS_VAR_IF([ac_cv_func_fopencookie], [yes],
  [AC_CACHE_CHECK([whether fopencookie seeker uses off64_t],
    [php_cv_type_cookie_off64_t],
    [AC_RUN_IFELSE([AC_LANG_SOURCE([
#include <stdio.h>
#include <stdlib.h>

struct cookiedata {
  off64_t pos;
};

ssize_t reader(void *cookie, char *buffer, size_t size)
{ (void)cookie; (void)buffer; return size; }
ssize_t writer(void *cookie, const char *buffer, size_t size)
{ (void)cookie; (void)buffer; return size; }
int closer(void *cookie)
{ (void)cookie; return 0; }
int seeker(void *cookie, off64_t *position, int whence)
{ ((struct cookiedata*)cookie)->pos = *position; (void)whence; return 0; }

cookie_io_functions_t funcs = {reader, writer, seeker, closer};

int main(void) {
  struct cookiedata g = { 0 };
  FILE *fp = fopencookie(&g, "r", funcs);
  if (fp && fseek(fp, 8192, SEEK_SET) == 0 && g.pos == 8192) {
    return 0;
  }
  return 1;
}
])],
  [php_cv_type_cookie_off64_t=yes],
  [php_cv_type_cookie_off64_t=no],
  [AS_CASE([$host_alias],
    [*linux*], [php_cv_type_cookie_off64_t=yes],
    [php_cv_type_cookie_off64_t=no])]
  )])
  AS_VAR_IF([php_cv_type_cookie_off64_t], [yes],
    [AC_DEFINE([COOKIE_SEEKER_USES_OFF64_T], [1],
      [Define to 1 if fopencookie seeker uses off64_t.])])
])
])

dnl ----------------------------------------------------------------------------
dnl Library/function existence and build sanity checks.
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_CHECK_LIBRARY(library, function [, action-found [, action-not-found [, extra-libs]]])
dnl
dnl Wrapper for AC_CHECK_LIB.
dnl
AC_DEFUN([PHP_CHECK_LIBRARY], [
  save_old_LDFLAGS=$LDFLAGS
  ac_stuff="$5"

  save_ext_shared=$ext_shared
  ext_shared=yes
  PHP_EVAL_LIBLINE([$]ac_stuff, LDFLAGS)
  AC_CHECK_LIB([$1],[$2],[
    LDFLAGS=$save_old_LDFLAGS
    ext_shared=$save_ext_shared
    $3
  ],[
    LDFLAGS=$save_old_LDFLAGS
    ext_shared=$save_ext_shared
    unset ac_cv_lib_$1[]_$2
    $4
  ])dnl
])

dnl
dnl PHP_CHECK_FRAMEWORK(framework, function [, action-found [, action-not-found ]])
dnl
dnl El cheapo wrapper for AC_CHECK_LIB.
dnl
AC_DEFUN([PHP_CHECK_FRAMEWORK], [
  save_old_LDFLAGS=$LDFLAGS
  LDFLAGS="-framework $1 $LDFLAGS"
  dnl Supplying "c" to AC_CHECK_LIB is technically cheating, but rewriting
  dnl AC_CHECK_LIB is overkill and this only affects the "checking.." output
  dnl anyway.
  AC_CHECK_LIB([c],[$2],[
    LDFLAGS=$save_old_LDFLAGS
    $3
  ],[
    LDFLAGS=$save_old_LDFLAGS
    $4
  ])
])

dnl
dnl PHP_CHECK_FUNC_LIB(func, libs)
dnl
dnl This macro checks whether 'func' or '__func' exists in the specified
dnl library. Defines HAVE_func and HAVE_library if found and adds the library to
dnl LIBS. This should be called in the ACTION-IF-NOT-FOUND part of
dnl PHP_CHECK_FUNC.
dnl
dnl autoconf undefines the builtin "shift" :-(
dnl If possible, we use the builtin shift anyway, otherwise we use the ubercool
dnl definition that has been tested so far with FreeBSD/GNU m4.
dnl
ifdef([builtin],[builtin(define, phpshift, [builtin(shift, $@)])],[
define([phpshift],[ifelse(index([$@],[,]),-1,,[substr([$@],incr(index([$@],[,])))])])
])

dnl
dnl PHP_CHECK_FUNC_LIB
dnl
AC_DEFUN([PHP_CHECK_FUNC_LIB],[
  ifelse($2,,:,[
  unset ac_cv_lib_$2[]_$1
  unset ac_cv_lib_$2[]___$1
  unset found
  AC_CHECK_LIB($2, $1, [found=yes], [
    AC_CHECK_LIB($2, __$1, [found=yes], [found=no])
  ])

  if test "$found" = "yes"; then
    ac_libs=$LIBS
    LIBS="$LIBS -l$2"
    AC_RUN_IFELSE([AC_LANG_PROGRAM()],[found=yes],[found=no],[
      dnl Cross compilation.
      found=yes
    ])
    LIBS=$ac_libs
  fi

  if test "$found" = "yes"; then
    PHP_ADD_LIBRARY($2)
    AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_$1]), [1],
      [Define to 1 if you have the '$1' function.])
    AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_LIB$2]), [1],
      [Define to 1 if you have the '$2' library (-l$2).])
    ac_cv_func_$1=yes
  else
    PHP_CHECK_FUNC_LIB($1,phpshift(phpshift($@)))
  fi
  ])
])

dnl
dnl PHP_CHECK_FUNC(func, ...)
dnl
dnl This macro checks whether 'func' or '__func' exists in the default libraries
dnl and as a fall back in the specified library. Defines HAVE_func and
dnl HAVE_library if found and adds the library to LIBS.
dnl
AC_DEFUN([PHP_CHECK_FUNC],[
  unset ac_cv_func_$1
  unset ac_cv_func___$1
  unset found

  AC_CHECK_FUNC($1, [found=yes],[ AC_CHECK_FUNC(__$1,[found=yes],[found=no]) ])

  case $found in
  yes[)]
    AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_$1]), [1],
      [Define to 1 if you have the '$1' function.])
    ac_cv_func_$1=yes
  ;;
  ifelse($#,1,,[
    *[)] PHP_CHECK_FUNC_LIB($@) ;;
  ])
  esac
])

dnl ----------------------------------------------------------------------------
dnl Platform characteristics checks.
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_SHLIB_SUFFIX_NAMES
dnl
dnl Determines link library suffix SHLIB_SUFFIX_NAME which can be: .so, .sl or
dnl .dylib
dnl
dnl Determines shared library suffix SHLIB_DL_SUFFIX_NAME suffix can be: .so or
dnl .sl
dnl
AC_DEFUN([PHP_SHLIB_SUFFIX_NAMES], [
AC_REQUIRE([PHP_CANONICAL_HOST_TARGET])dnl
AS_CASE([$host_alias],
  [*hpux*], [
    SHLIB_SUFFIX_NAME=sl
    SHLIB_DL_SUFFIX_NAME=sl
  ],
  [*darwin*], [
    SHLIB_SUFFIX_NAME=dylib
    SHLIB_DL_SUFFIX_NAME=so
  ], [
    SHLIB_SUFFIX_NAME=so
    SHLIB_DL_SUFFIX_NAME=$SHLIB_SUFFIX_NAME
  ])
PHP_SUBST_OLD([SHLIB_SUFFIX_NAME])
PHP_SUBST_OLD([SHLIB_DL_SUFFIX_NAME])
])

dnl
dnl PHP_C_BIGENDIAN
dnl
dnl Replacement macro for AC_C_BIGENDIAN.
dnl
AC_DEFUN([PHP_C_BIGENDIAN],
[AC_CACHE_CHECK([whether byte ordering is bigendian], [ac_cv_c_bigendian_php],
[AC_RUN_IFELSE([AC_LANG_SOURCE([
int main(void)
{
  short one = 1;
  char *cp = (char *)&one;

  if (*cp == 0) {
    return 0;
  }

  return 1;
}
])],
  [ac_cv_c_bigendian_php=yes],
  [ac_cv_c_bigendian_php=no],
  [ac_cv_c_bigendian_php=unknown])])
AS_VAR_IF([ac_cv_c_bigendian_php], [yes],
  [AC_DEFINE([WORDS_BIGENDIAN], [],
    [Define if processor uses big-endian word.])])
])

dnl ----------------------------------------------------------------------------
dnl Checks for programs: PHP_PROG_<program>.
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_PROG_SENDMAIL
dnl
dnl Search for the sendmail binary.
dnl
AC_DEFUN([PHP_PROG_SENDMAIL],
[PHP_ALT_PATH=/usr/bin:/usr/sbin:/usr/etc:/etc:/usr/ucblib:/usr/lib
AC_PATH_PROG([PROG_SENDMAIL], [sendmail], [], [$PATH:$PHP_ALT_PATH])
AS_VAR_IF([PROG_SENDMAIL],, [PROG_SENDMAIL=/usr/sbin/sendmail
AC_MSG_NOTICE([default sendmail_path INI directive set to $PROG_SENDMAIL])])])

dnl
dnl PHP_PROG_AWK
dnl
dnl Some vendors force mawk before gawk; mawk is broken so we don't like that.
dnl
AC_DEFUN([PHP_PROG_AWK], [
AC_CHECK_PROGS([AWK], [gawk nawk awk mawk], [bork], [/usr/xpg4/bin/:$PATH])
AS_CASE([$AWK],
  [*mawk],
    [AC_MSG_WARN(m4_text_wrap([
      mawk is known to have problems on some systems. You should install GNU awk
    ]))],
  [*gawk], [],
  [bork], [AC_MSG_ERROR([Could not find awk; Install GNU awk])],
  [
    AC_MSG_CHECKING([if $AWK is broken])
    AS_IF([! $AWK 'function foo() {}' >/dev/null 2>&1], [
      AC_MSG_RESULT([yes])
      AC_MSG_ERROR([You should install GNU awk])
    ],
    [AC_MSG_RESULT([no])])
  ])
PHP_SUBST([AWK])
])

dnl
dnl PHP_PROG_BISON([MIN-VERSION], [EXCLUDED-VERSION...])
dnl
dnl Search for bison and optionally check if version is at least the minimum
dnl required version MIN-VERSION and doesn't match any of the blank separated
dnl list of excluded versions EXCLUDED-VERSION (for example "3.0 3.2").
dnl
AC_DEFUN([PHP_PROG_BISON], [
  AC_CHECK_PROG([YACC], [bison], [bison])

  ifelse($1,,php_bison_required_version='',php_bison_required_version="$1")
  ifelse($2,,php_bison_excluded_versions='none',php_bison_excluded_versions="$2")

  if test -n "$YACC"; then
    AC_MSG_CHECKING([for bison version])

    php_bison_version=$($YACC --version 2> /dev/null | grep 'GNU Bison' | cut -d ' ' -f 4 | tr -d a-z)
    if test -z "$php_bison_version"; then
      php_bison_version=0.0.0
    fi
    ac_IFS=$IFS; IFS="."
    set $php_bison_version
    IFS=$ac_IFS
    php_bison_num=$(expr [$]{1:-0} \* 10000 + [$]{2:-0} \* 100 + [$]{3:-0})
    php_bison_branch="[$]1.[$]2"
    php_bison_check=ok

    if test -z "$php_bison_required_version" && test -z "$php_bison_num"; then
      php_bison_check=invalid
    elif test -n "$php_bison_required_version"; then
      ac_IFS=$IFS; IFS="."
      set $php_bison_required_version
      IFS=$ac_IFS
      php_bison_required_num=$(expr [$]{1:-0} \* 10000 + [$]{2:-0} \* 100 + [$]{3:-0})
      php_bison_required_version="$php_bison_required_version or later"

      if test -z "$php_bison_num" || test "$php_bison_num" -lt "$php_bison_required_num"; then
        php_bison_check=invalid
      fi
    fi

    for php_bison_check_version in $php_bison_excluded_versions; do
      if test "$php_bison_version" = "$php_bison_check_version" || test "$php_bison_branch" = "$php_bison_check_version"; then
        php_bison_check=invalid
        break
      fi
    done

    if test "$php_bison_check" != "invalid"; then
      PHP_SUBST_OLD([YFLAGS], [-Wall])
      AC_MSG_RESULT([$php_bison_version (ok)])
    else
      AC_MSG_RESULT([$php_bison_version])
    fi
  fi

  case $php_bison_check in
    ""|invalid[)]
      if test ! -f "$abs_srcdir/Zend/zend_language_parser.h" || test ! -f "$abs_srcdir/Zend/zend_language_parser.c"; then
        AC_MSG_ERROR(m4_text_wrap([
          bison $php_bison_required_version or newer is required to generate PHP
          parsers (excluded versions: $php_bison_excluded_versions).
        ]))
      fi

      YACC="exit 0;"
      ;;
  esac

  PHP_SUBST([YACC])
])

dnl
dnl PHP_PROG_RE2C([min-version], [options])
dnl
dnl Search for the re2c and optionally check if version is at least the minimum
dnl required version "min-version". The blank-or-newline-separated "options" are
dnl the re2c command-line flags substituted into a Makefile variable RE2C_FLAGS
dnl which can be added to all re2c invocations.
dnl
AC_DEFUN([PHP_PROG_RE2C],[
  AC_CHECK_PROG([RE2C], [re2c], [re2c])

  php_re2c_check=
  AS_VAR_IF([RE2C],,, [
    AC_MSG_CHECKING([for re2c version])

    php_re2c_version=$($RE2C --version | cut -d ' ' -f 2 2>/dev/null)

    php_re2c_check=ok
    AS_VERSION_COMPARE([$php_re2c_version], [$1],
      [php_re2c_check=invalid])

    AS_VAR_IF([php_re2c_check], [invalid],
      [AC_MSG_RESULT([$php_re2c_version (too old)])],
      [AC_MSG_RESULT([$php_re2c_version (ok)])])
  ])

  AS_CASE([$php_re2c_check],
    [""|invalid], [
      AS_IF([test ! -f "$abs_srcdir/Zend/zend_language_scanner.c"], [
        AC_MSG_ERROR(m4_text_wrap([
          re2c $1 or newer is required to generate PHP lexers.
        ]))
      ])

      RE2C="exit 0;"
    ])

  PHP_SUBST([RE2C])
  AS_VAR_SET([RE2C_FLAGS], m4_normalize(["$2"]))
  PHP_SUBST([RE2C_FLAGS])
])

dnl
dnl PHP_PROG_PHP([min-version])
dnl
dnl Find PHP command-line interface SAPI on the system and check if version is
dnl greater or equal to "min-version". If suitable version is found, the PHP
dnl variable is set and substituted to a Makefile variable. Used for generating
dnl files and running PHP utilities during the build.
dnl
AC_DEFUN([PHP_PROG_PHP],
[m4_if([$1],, [php_required_version=7.4], [php_required_version=$1])
AC_CHECK_PROG([PHP], [php], [php])
AS_VAR_IF([PHP],,, [
AC_MSG_CHECKING([for php version])
php_version=$($PHP -v | head -n1 | cut -d ' ' -f 2)
AS_VERSION_COMPARE([$php_version], [$php_required_version], [unset PHP])
AS_VAR_IF([PHP],,
  [AC_MSG_RESULT([$php_version (too old, install $php_required_version or later)])],
  [AC_MSG_RESULT([$php_version (ok)])])
])
PHP_SUBST([PHP])
])

dnl ----------------------------------------------------------------------------
dnl Common setup macros: PHP_SETUP_<what>
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_SETUP_ICU([shared-add])
dnl
dnl Common setup macro for ICU.
dnl
AC_DEFUN([PHP_SETUP_ICU],[
  PKG_CHECK_MODULES([ICU], [icu-uc >= 57.1 icu-io icu-i18n])

  PHP_EVAL_INCLINE([$ICU_CFLAGS])
  PHP_EVAL_LIBLINE([$ICU_LIBS], [$1])

  ICU_CFLAGS="$ICU_CFLAGS -DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1"
  ICU_CXXFLAGS="$ICU_CXXFLAGS -DUNISTR_FROM_CHAR_EXPLICIT=explicit -DUNISTR_FROM_STRING_EXPLICIT=explicit"

  AS_IF([$PKG_CONFIG icu-io --atleast-version=60],
    [ICU_CFLAGS="$ICU_CFLAGS -DU_HIDE_OBSOLETE_UTF_OLD_H=1"])
])

dnl
dnl PHP_SETUP_OPENSSL(shared-add [, action-found])
dnl
dnl Common setup macro for openssl.
dnl
AC_DEFUN([PHP_SETUP_OPENSSL],[
  found_openssl=no

  PKG_CHECK_MODULES([OPENSSL], [openssl >= 1.1.1], [found_openssl=yes])

  if test "$found_openssl" = "yes"; then
    PHP_EVAL_LIBLINE([$OPENSSL_LIBS], [$1])
    PHP_EVAL_INCLINE([$OPENSSL_CFLAGS])
ifelse([$2],[],:,[$2])
  fi
])

dnl
dnl PHP_SETUP_ICONV(shared-add [, action-found [, action-not-found]])
dnl
dnl Common setup macro for iconv.
dnl
AC_DEFUN([PHP_SETUP_ICONV], [
  found_iconv=no
  unset ICONV_DIR
  AH_TEMPLATE([HAVE_LIBICONV],
    [Define to 1 if you have the 'libiconv' function.])

  dnl Check libc first if no path is provided in --with-iconv.
  AS_VAR_IF([PHP_ICONV], [yes], [
    dnl Reset LIBS temporarily as it may have already been included -liconv in.
    LIBS_save=$LIBS
    LIBS=
    AC_CHECK_FUNC([iconv], [found_iconv=yes],
      [AC_CHECK_FUNC([libiconv], [
        AC_DEFINE([HAVE_LIBICONV], [1])
        found_iconv=yes
      ])])
    LIBS=$LIBS_save
  ])

  dnl Check external libs for iconv funcs.
  AS_VAR_IF([found_iconv], [no], [

  dnl Find /opt/homebrew/opt/libiconv on macOS
  dnl See: https://github.com/php/php-src/pull/19475
    php_brew_prefix=no
    AC_CHECK_PROG([BREW], [brew], [brew])
    if test -n "$BREW"; then
      AC_MSG_CHECKING([for homebrew prefix])
      php_brew_prefix=$($BREW --prefix 2> /dev/null)
    fi

    for i in $PHP_ICONV $php_brew_prefix/opt/libiconv /usr/local /usr; do
      if test -r $i/include/gnu-libiconv/iconv.h; then
        ICONV_DIR=$i
        ICONV_INCLUDE_DIR=$i/include/gnu-libiconv
        iconv_lib_name=iconv
        break
      elif test -r $i/include/iconv.h; then
        ICONV_DIR=$i
        ICONV_INCLUDE_DIR=$i/include
        iconv_lib_name=iconv
        break
      fi
    done

    AS_VAR_IF([ICONV_DIR],,
      [AC_MSG_ERROR(m4_text_wrap([
        Please specify the install prefix of iconv with --with-iconv=<DIR>
      ]))])

    if test -f $ICONV_DIR/$PHP_LIBDIR/lib$iconv_lib_name.a ||
       test -f $ICONV_DIR/$PHP_LIBDIR/lib$iconv_lib_name.$SHLIB_SUFFIX_NAME ||
       test -f $ICONV_DIR/$PHP_LIBDIR/lib$iconv_lib_name.tbd
    then
      PHP_CHECK_LIBRARY([$iconv_lib_name], [libiconv], [
        found_iconv=yes
        AC_DEFINE([HAVE_LIBICONV], [1])
        AC_DEFINE([ICONV_ALIASED_LIBICONV], [1],
          [Define to 1 if 'iconv()' is aliased to 'libiconv()'.])
        ],
        [PHP_CHECK_LIBRARY([$iconv_lib_name], [iconv],
          [found_iconv=yes],
          [],
          [-L$ICONV_DIR/$PHP_LIBDIR])],
        [-L$ICONV_DIR/$PHP_LIBDIR])
    fi
  ])

  if test "$found_iconv" = "yes"; then
    if test -n "$ICONV_DIR"; then
      PHP_ADD_LIBRARY_WITH_PATH([$iconv_lib_name],
        [$ICONV_DIR/$PHP_LIBDIR],
        [$1])
      PHP_ADD_INCLUDE([$ICONV_INCLUDE_DIR])
    fi
    $2
ifelse([$3],[],,[else $3])
  fi
])

dnl
dnl PHP_SETUP_LIBXML(shared-add [, action-found])
dnl
dnl Common setup macro for libxml.
dnl
AC_DEFUN([PHP_SETUP_LIBXML], [
  PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= 2.9.4])
  PHP_EVAL_INCLINE([$LIBXML_CFLAGS])
  PHP_EVAL_LIBLINE([$LIBXML_LIBS], [$1])
  $2
])

dnl
dnl PHP_SETUP_EXPAT([shared-add])
dnl
dnl Common setup macro for expat.
dnl
AC_DEFUN([PHP_SETUP_EXPAT], [
  PKG_CHECK_MODULES([EXPAT], [expat])
  PHP_EVAL_INCLINE([$EXPAT_CFLAGS])
  PHP_EVAL_LIBLINE([$EXPAT_LIBS], [$1])
  AC_DEFINE([HAVE_LIBEXPAT], [1],
    [Define to 1 if the system has the Expat XML parser library.])
])

dnl
dnl PHP_SETUP_SQLITE([shared-add])
dnl
dnl Common setup macro for SQLite library.
dnl
AC_DEFUN([PHP_SETUP_SQLITE], [
PKG_CHECK_MODULES([SQLITE], [sqlite3 >= 3.7.17], [
  PHP_EVAL_INCLINE([$SQLITE_CFLAGS])
  PHP_EVAL_LIBLINE([$SQLITE_LIBS], [$1])
])
])

dnl
dnl PHP_SETUP_ZLIB(shared-add [, action-found, [action-not-found]])
dnl
dnl Common setup macro for zlib library. If zlib is not found on the system, the
dnl default error by PKG_CHECK_MODULES is emitted, unless the action-not-found
dnl is given.
dnl
AC_DEFUN([PHP_SETUP_ZLIB], [dnl
PKG_CHECK_MODULES([ZLIB], [zlib >= 1.2.11], [dnl
  PHP_EVAL_INCLINE([$ZLIB_CFLAGS])
  PHP_EVAL_LIBLINE([$ZLIB_LIBS], [$1])
  $2], [$3])dnl
])

dnl
dnl PHP_SETUP_PGSQL([shared-add [, action-found [, action-not-found [, pgsql-dir]]]])
dnl
dnl Common setup macro for PostgreSQL library (libpq). The optional "pgsql-dir"
dnl is the PostgreSQL base install directory or the path to pg_config. Support
dnl for pkg-config was introduced in PostgreSQL 9.3. If library can't be found
dnl with pkg-config, check falls back to pg_config. If libpq is not found, error
dnl is thrown, unless the "action-not-found" is given.
dnl
AC_DEFUN([PHP_SETUP_PGSQL], [dnl
found_pgsql=no
dnl Set PostgreSQL installation directory if given from the configure argument.
AS_CASE([$4], [yes], [pgsql_dir=""], [pgsql_dir=$4])
AS_VAR_IF([pgsql_dir],,
  [PKG_CHECK_MODULES([PGSQL], [libpq >= 10.0],
    [found_pgsql=yes],
    [found_pgsql=no])])

AS_VAR_IF([found_pgsql], [no], [dnl
  AC_MSG_CHECKING([for pg_config])
  for i in $pgsql_dir $pgsql_dir/bin /usr/local/pgsql/bin /usr/local/bin /usr/bin ""; do
    AS_IF([test -x $i/pg_config], [PG_CONFIG="$i/pg_config"; break;])
  done

  AS_VAR_IF([PG_CONFIG],, [dnl
    AC_MSG_RESULT([not found])
    AS_VAR_IF([pgsql_dir],,
      [pgsql_search_paths="/usr /usr/local /usr/local/pgsql"],
      [pgsql_search_paths=$pgsql_dir])

    for i in $pgsql_search_paths; do
      for j in include include/pgsql include/postgres include/postgresql ""; do
        AS_IF([test -r "$i/$j/libpq-fe.h"], [PGSQL_INCLUDE=$i/$j])
      done

      for j in $PHP_LIBDIR lib $PHP_LIBDIR/pgsql $PHP_LIBDIR/postgres $PHP_LIBDIR/postgresql ""; do
        AS_IF([test -f "$i/$j/libpq.so" || test -f "$i/$j/libpq.a"],
          [PGSQL_LIBDIR=$i/$j])
      done
    done
  ], [dnl
    AC_MSG_RESULT([$PG_CONFIG])
    PGSQL_INCLUDE=$($PG_CONFIG --includedir)
    PGSQL_LIBDIR=$($PG_CONFIG --libdir)
  ])

  AS_IF([test -n "$PGSQL_INCLUDE" && test -n "PGSQL_LIBDIR"], [
    found_pgsql=yes
    PGSQL_CFLAGS="-I$PGSQL_INCLUDE"
    PGSQL_LIBS="-L$PGSQL_LIBDIR -lpq"
  ])dnl
])

AS_VAR_IF([found_pgsql], [yes], [dnl
  PHP_EVAL_INCLINE([$PGSQL_CFLAGS])
  PHP_EVAL_LIBLINE([$PGSQL_LIBS], [$1])
dnl PostgreSQL minimum version sanity check.
  PHP_CHECK_LIBRARY([pq], [PQencryptPasswordConn],,
    [AC_MSG_FAILURE([PostgreSQL check failed: libpq 10.0 or later is required.])],
  [$PGSQL_LIBS])
$2],
[m4_default([$3], [AC_MSG_FAILURE(m4_text_wrap([
  Cannot find libpq-fe.h or pq library (libpq). Please specify the correct
  PostgreSQL installation path with environment variables PGSQL_CFLAGS and
  PGSQL_LIBS or provide the PostgreSQL installation directory.
]))])])
])

dnl ----------------------------------------------------------------------------
dnl Misc. macros
dnl ----------------------------------------------------------------------------

dnl
dnl PHP_INSTALL_HEADERS(path [, files ...])
dnl
dnl Add PHP header files to the list to be installed on the system. The "files"
dnl argument is a blank-or-newline-separated list of header files or directories
dnl located in the "path". If 2nd argument is not given, it installs header
dnl files in all paths from the blank-or-newline-separated "path" argument.
dnl
AC_DEFUN([PHP_INSTALL_HEADERS],
  [m4_ifblank([$2], [
    for header_file in m4_normalize(m4_expand([$1])); do
      PHP_RUN_ONCE([INSTALLHEADERS], [$header_file],
        [INSTALL_HEADERS="$INSTALL_HEADERS $header_file"])
    done
  ], [
    header_path=$1
    for header_file in m4_normalize(m4_expand([$2])); do
      hp_hf="$header_path/$header_file"
      PHP_RUN_ONCE([INSTALLHEADERS], [$hp_hf],
        [INSTALL_HEADERS="$INSTALL_HEADERS $hp_hf"])
    done
  ])
])

dnl
dnl PHP_CONFIG_NICE(filename)
dnl
dnl This macro creates script file with given filename which includes the last
dnl configure command run by user. This file is named 'config.nice' in PHP.
dnl
AC_DEFUN([PHP_CONFIG_NICE],[
  AC_REQUIRE([AC_PROG_EGREP])
  AC_REQUIRE([AC_PROG_SED])
  PHP_SUBST_OLD([EGREP])
  PHP_SUBST_OLD([SED])
  test -f $1 && mv $1 $1.old
  rm -f $1.old
  cat >$1<<EOF
#! /bin/sh
#
# Created by configure

EOF

  clean_configure_args=$ac_configure_args
  for var in CFLAGS CXXFLAGS CPPFLAGS LDFLAGS EXTRA_LDFLAGS_PROGRAM LIBS CC CXX; do
    eval val=\$$var
    if test -n "$val"; then
      echo "$var='$val' \\" >> $1
      if test $(expr "X$ac_configure_args" : ".*${var}.*") != 0; then
        clean_configure_args=$(echo $clean_configure_args | $SED -e "s#'$var=$val'##")
      fi
    fi
  done

  echo "'[$]0' \\" >> $1
  if test $(expr " [$]0" : " '.*") = 0; then
    CONFIGURE_COMMAND="$CONFIGURE_COMMAND '[$]0'"
  else
    CONFIGURE_COMMAND="$CONFIGURE_COMMAND [$]0"
  fi
  CONFIGURE_ARGS="$clean_configure_args"
  while test "X$CONFIGURE_ARGS" != "X";
  do
   if CURRENT_ARG=$(expr "X$CONFIGURE_ARGS" : "X *\('[[^']]*'\)")
   then
     CONFIGURE_ARGS=$(expr "X$CONFIGURE_ARGS" : "X *'[[^']]*' \(.*\)")
   elif CURRENT_ARG=$(expr "X$CONFIGURE_ARGS" : "X *\([[^ ]]*\)")
   then
     CONFIGURE_ARGS=$(expr "X$CONFIGURE_ARGS" : "X *[[^ ]]* \(.*\)")
     CURRENT_ARG="'$CURRENT_ARG'"
   else
    break
   fi
   AS_ECHO(["$CURRENT_ARG \\"]) >>$1
   CONFIGURE_OPTIONS="$CONFIGURE_OPTIONS $CURRENT_ARG"
  done
  echo '"[$]@"' >> $1
  chmod +x $1
  CONFIGURE_COMMAND="$CONFIGURE_COMMAND $CONFIGURE_OPTIONS"
  AC_SUBST([CONFIGURE_COMMAND])
  AC_SUBST([CONFIGURE_OPTIONS])
])

dnl
dnl PHP_CHECK_PDO_INCLUDES([found [, not-found]])
dnl
AC_DEFUN([PHP_CHECK_PDO_INCLUDES],[
  AC_CACHE_CHECK([for PDO includes], pdo_cv_inc_path, [
    if test -f $abs_srcdir/ext/pdo/php_pdo_driver.h; then
      pdo_cv_inc_path=$abs_srcdir/ext
    elif test -f $phpincludedir/ext/pdo/php_pdo_driver.h; then
      pdo_cv_inc_path=$phpincludedir/ext
    fi
  ])
  if test -n "$pdo_cv_inc_path"; then
ifelse([$1],[],:,[$1])
  else
ifelse([$2],[],[AC_MSG_ERROR([Cannot find php_pdo_driver.h.])],[$2])
  fi
])

dnl
dnl PHP_DETECT_ICC
dnl
dnl Detect Intel C++ Compiler and unset $GCC if ICC found.
dnl
AC_DEFUN([PHP_DETECT_ICC],
[
  ICC="no"
  AC_MSG_CHECKING([for icc])
  AC_EGREP_CPP([^__INTEL_COMPILER], [__INTEL_COMPILER],
    ICC="no"
    AC_MSG_RESULT([no]),
    ICC="yes"
    GCC="no"
    AC_MSG_RESULT([yes])
  )
])

dnl
dnl PHP_DETECT_SUNCC
dnl
dnl Detect if the systems default compiler is suncc. We also set some useful
dnl CFLAGS if the user didn't set any.
dnl
AC_DEFUN([PHP_DETECT_SUNCC],[
  SUNCC="no"
  AC_MSG_CHECKING([for suncc])
  AC_EGREP_CPP([^__SUNPRO_C], [__SUNPRO_C],
    SUNCC="no"
    AC_MSG_RESULT([no]),
    SUNCC="yes"
    GCC="no"
    test -n "$auto_cflags" && CFLAGS="-O -xs -xstrconst -zlazyload"
    GCC=""
    AC_MSG_RESULT([yes])
  )
])

dnl
dnl PHP_CRYPT_R_STYLE
dnl
dnl Detect the style of crypt_r() if any is available.
dnl See APR_CHECK_CRYPT_R_STYLE() for original version.
dnl
AC_DEFUN([PHP_CRYPT_R_STYLE],
[AC_CACHE_CHECK([which data struct is used by crypt_r], [php_cv_crypt_r_style],
[php_cv_crypt_r_style=none
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
#define _REENTRANT 1
#include <crypt.h>
], [
CRYPTD buffer;
crypt_r("passwd", "hash", &buffer);
])], [php_cv_crypt_r_style=cryptd], [])

AS_VAR_IF([php_cv_crypt_r_style], [none],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
#define _REENTRANT 1
#include <crypt.h>
],[
struct crypt_data buffer;
crypt_r("passwd", "hash", &buffer);
])], [php_cv_crypt_r_style=struct_crypt_data], [])])

AS_VAR_IF([php_cv_crypt_r_style], [none],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
#define _REENTRANT 1
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <crypt.h>
],[
struct crypt_data buffer;
crypt_r("passwd", "hash", &buffer);
])], [php_cv_crypt_r_style=struct_crypt_data_gnu_source], [])])

AS_VAR_IF([php_cv_crypt_r_style], [none],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
#include <stdlib.h>
#include <unistd.h>
],[
struct crypt_data buffer;
crypt_r("passwd", "hash", &buffer);
])], [php_cv_crypt_r_style=struct_crypt_data], [])])
])

AS_VAR_IF([php_cv_crypt_r_style], [cryptd],
  [AC_DEFINE([CRYPT_R_CRYPTD], [1], [Define to 1 if crypt_r uses CRYPTD.])])

AS_CASE([$php_cv_crypt_r_style],
  [struct_crypt_data|struct_crypt_data_gnu_source],
    [AC_DEFINE([CRYPT_R_STRUCT_CRYPT_DATA], [1],
      [Define to 1 if crypt_r uses struct crypt_data.])])

AS_VAR_IF([php_cv_crypt_r_style], [struct_crypt_data_gnu_source],
  [AC_DEFINE([CRYPT_R_GNU_SOURCE], [1],
    [Define to 1 if struct crypt_data requires _GNU_SOURCE.])])

AS_VAR_IF([php_cv_crypt_r_style], [none],
  [AC_MSG_FAILURE([Unable to detect data struct used by crypt_r.])])
])

dnl
dnl PHP_TEST_WRITE_STDOUT
dnl
AC_DEFUN([PHP_TEST_WRITE_STDOUT],
[AC_CACHE_CHECK([whether writing to stdout works], [php_cv_have_write_stdout],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define TEXT "This is the test message -- "

int main(void)
{
  int n;

  n = write(1, TEXT, sizeof(TEXT)-1);
  return (!(n == sizeof(TEXT)-1));
}
    ])],
    [php_cv_have_write_stdout=yes],
    [php_cv_have_write_stdout=no],
    [AS_CASE([$host_alias],
      [*linux*|*midipix], [php_cv_have_write_stdout=yes],
      [php_cv_have_write_stdout=no])])])
AS_VAR_IF([php_cv_have_write_stdout], [yes],
  [AC_DEFINE([PHP_WRITE_STDOUT], [1], [Define to 1 if 'write(2)' works.])])
])

dnl
dnl PHP_INIT_DTRACE(providerdesc, header-file, sources [, module])
dnl
dnl Initialize the DTrace support using the DTrace "providerdesc" file and
dnl generate "header-file". The "sources" is a blank-or-newline-separated list
dnl of files. The optional "module" is PHP extension name or path when used in
dnl extensions.
dnl
AC_DEFUN([PHP_INIT_DTRACE],
[AC_CHECK_HEADER([sys/sdt.h],,
  [AC_MSG_FAILURE([Cannot find required <sys/sdt.h> to enable DTrace support.])])

dnl Set paths properly when called from extension.
  case "$4" in
    ""[)] ac_srcdir="$abs_srcdir/"; unset ac_bdir;;
    /*[)] ac_srcdir=$(echo "$4"|cut -c 2-)"/"; ac_bdir=$ac_srcdir;;
    *[)] ac_srcdir="$abs_srcdir/$1/"; ac_bdir="$4/";;
  esac

dnl providerdesc.
  ac_provsrc=$1
  old_IFS=[$]IFS
  IFS=.
  set $ac_provsrc
  ac_provobj=[$]1
  IFS=$old_IFS

dnl header-file.
  ac_hdrobj=$2

dnl Add providerdesc.o or .lo into global objects when needed.
  case $host_alias in
  *freebsd*)
    PHP_GLOBAL_OBJS="[$]PHP_GLOBAL_OBJS [$]ac_bdir[$]ac_provsrc.lo"
    PHP_LDFLAGS="$PHP_LDFLAGS -lelf"
    ;;
  *solaris*)
    PHP_GLOBAL_OBJS="[$]PHP_GLOBAL_OBJS [$]ac_bdir[$]ac_provsrc.lo"
    ;;
  *linux*)
    PHP_GLOBAL_OBJS="[$]PHP_GLOBAL_OBJS [$]ac_bdir[$]ac_provsrc.lo"
    ;;
  esac

dnl DTrace objects.
  old_IFS=[$]IFS
  for ac_src in m4_normalize([$3]); do
    IFS=.
    set $ac_src
    ac_obj=[$]1
    IFS=$old_IFS

    PHP_DTRACE_OBJS="[$]PHP_DTRACE_OBJS [$]ac_bdir[$]ac_obj.lo"
  done;

  case [$]php_sapi_module in
  shared[)]
    for ac_lo in $PHP_DTRACE_OBJS; do
      dtrace_objs="[$]dtrace_objs $(echo $ac_lo | $SED -e 's,\.lo$,.o,' -e 's#\(.*\)\/#\1\/.libs\/#')"
    done;
    ;;
  *[)]
    dtrace_objs='$(PHP_DTRACE_OBJS:.lo=.o)'
    ;;
  esac

dnl Generate Makefile.objects entries. The empty $ac_provsrc command stops an
dnl implicit circular dependency in GNU Make which causes the .d file to be
dnl overwritten (Bug 61268).
  cat>>Makefile.objects<<EOF

$abs_srcdir/$ac_provsrc:;

$ac_bdir[$]ac_hdrobj: $abs_srcdir/$ac_provsrc
	CC="\$(CC)" CFLAGS="\$(CFLAGS_CLEAN)" dtrace -h -C -s $ac_srcdir[$]ac_provsrc -o \$[]@.bak && \$(SED) -e 's,PHP_,DTRACE_,g' \$[]@.bak > \$[]@

\$(PHP_DTRACE_OBJS): $ac_bdir[$]ac_hdrobj

EOF

  case $host_alias in
  *solaris*|*linux*|*freebsd*)
    dtrace_prov_name="$(echo $ac_provsrc | $SED -e 's#\(.*\)\/##').o"
    dtrace_lib_dir="$(echo $ac_bdir[$]ac_provsrc | $SED -e 's#\(.*\)/[^/]*#\1#')/.libs"
    dtrace_d_obj="$(echo $ac_bdir[$]ac_provsrc | $SED -e 's#\(.*\)/\([^/]*\)#\1/.libs/\2#').o"
    dtrace_nolib_objs='$(PHP_DTRACE_OBJS:.lo=.o)'
    for ac_lo in $PHP_DTRACE_OBJS; do
      dtrace_lib_objs="[$]dtrace_lib_objs $(echo $ac_lo | $SED -e 's,\.lo$,.o,' -e 's#\(.*\)\/#\1\/.libs\/#')"
    done;
    dnl Always attempt to create both PIC and non-PIC DTrace objects (Bug 63692)
    cat>>Makefile.objects<<EOF
$ac_bdir[$]ac_provsrc.lo: \$(PHP_DTRACE_OBJS)
	echo "[#] Generated by Makefile for libtool" > \$[]@
	@test -d "$dtrace_lib_dir" || mkdir $dtrace_lib_dir
	if CC="\$(CC)" CFLAGS="\$(CFLAGS_CLEAN)" dtrace -G -o $dtrace_d_obj -s $abs_srcdir/$ac_provsrc $dtrace_lib_objs 2> /dev/null && test -f "$dtrace_d_obj"; then [\\]
	  echo "pic_object=['].libs/$dtrace_prov_name[']" >> \$[]@ [;\\]
	else [\\]
	  echo "pic_object='none'" >> \$[]@ [;\\]
	fi
	if CC="\$(CC)" CFLAGS="\$(CFLAGS_CLEAN)" dtrace -G -o $ac_bdir[$]ac_provsrc.o -s $abs_srcdir/$ac_provsrc $dtrace_nolib_objs 2> /dev/null && test -f "$ac_bdir[$]ac_provsrc.o"; then [\\]
	  echo "non_pic_object=[']$dtrace_prov_name[']" >> \$[]@ [;\\]
	else [\\]
	  echo "non_pic_object='none'" >> \$[]@ [;\\]
	fi

EOF

    ;;
  *)
cat>>Makefile.objects<<EOF
$ac_bdir[$]ac_provsrc.o: \$(PHP_DTRACE_OBJS)
	CC="\$(CC)" CFLAGS="\$(CFLAGS_CLEAN)" dtrace -G -o \$[]@ -s $abs_srcdir/$ac_provsrc $dtrace_objs

EOF
    ;;
  esac

PHP_SUBST([PHP_DTRACE_OBJS])
])

dnl
dnl PHP_CHECK_STDINT_TYPES
dnl
AC_DEFUN([PHP_CHECK_STDINT_TYPES], [
  AC_CHECK_SIZEOF([int])
  AC_CHECK_SIZEOF([long])
  AC_CHECK_SIZEOF([long long])
  AC_CHECK_SIZEOF([size_t])
  AC_CHECK_SIZEOF([off_t])
])

dnl
dnl PHP_CHECK_BUILTIN(builtin)
dnl
dnl Check GNU C builtin "builtin" and define the CPP macro
dnl PHP_HAVE_<BUILTIN_WITHOUT_LEADING_UNDERSCORES> to 1 if found.
dnl
AC_DEFUN([PHP_CHECK_BUILTIN],
[AS_VAR_PUSHDEF([php_var], [php_cv_have_$1])
AC_CACHE_CHECK([for $1], [php_var],
[AC_LINK_IFELSE([AC_LANG_SOURCE([int main(void) { m4_case([$1],
  [__builtin_clz], [return $1(1) ? 1 : 0;],
  [__builtin_clzl], [return $1(1) ? 1 : 0;],
  [__builtin_clzll], [return $1(1) ? 1 : 0;],
  [__builtin_cpu_init], [$1();],
  [__builtin_cpu_supports], [return $1("sse")? 1 : 0;],
  [__builtin_ctzl], [return $1(2L) ? 1 : 0;],
  [__builtin_ctzll], [return $1(2LL) ? 1 : 0;],
  [__builtin_expect], [return $1(1,1) ? 1 : 0;],
  [__builtin_frame_address], [return $1(0) != (void*)0;],
  [__builtin_saddl_overflow], [long tmpvar; return $1(3, 7, &tmpvar);],
  [__builtin_saddll_overflow], [long long tmpvar; return $1(3, 7, &tmpvar);],
  [__builtin_smull_overflow], [long tmpvar; return $1(3, 7, &tmpvar);],
  [__builtin_smulll_overflow], [long long tmpvar; return $1(3, 7, &tmpvar);],
  [__builtin_ssubl_overflow], [long tmpvar; return $1(3, 7, &tmpvar);],
  [__builtin_ssubll_overflow], [long long tmpvar; return $1(3, 7, &tmpvar);],
  [__builtin_unreachable], [$1();],
  [__builtin_usub_overflow], [unsigned int tmpvar; return $1(3, 7, &tmpvar);],
  [
    m4_warn([syntax], [Unsupported builtin '$1', the test may fail.])
    $1();
  ]) return 0; }])],
[AS_VAR_SET([php_var], [yes])],
[AS_VAR_SET([php_var], [no])])
])
AS_VAR_IF([php_var], [yes],
  [AC_DEFINE_UNQUOTED(AS_TR_CPP([PHP_HAVE_]m4_bpatsubst([$1], [^_*], [])), [1],
    [Define to 1 if the compiler supports '$1'.])])
AS_VAR_POPDEF([php_var])
])

dnl
dnl PHP_PATCH_CONFIG_HEADERS([FILE])
dnl
dnl PACKAGE_* symbols are automatically defined by Autoconf. When including
dnl configuration header, warnings about redefined symbols are emitted for such
dnl symbols if they are defined by multiple libraries. This disables all
dnl PACKAGE_* symbols in the generated configuration header template FILE. For
dnl example, main/php_config.h.in for PHP or config.h.in for PHP extensions.
dnl
AC_DEFUN([PHP_PATCH_CONFIG_HEADERS], [
  AC_MSG_NOTICE([patching $1])

  $SED -e 's/^#undef PACKAGE_[^ ]*/\/\* & \*\//g' < $srcdir/$1 \
    > $srcdir/$1.tmp && mv $srcdir/$1.tmp $srcdir/$1
])

dnl
dnl PHP_CHECK_AVX512_SUPPORTS
dnl
dnl Check whether the compiler supports the AVX-512 extensions and define
dnl PHP_HAVE_AVX512_SUPPORTS to 1 if found. Note that this is a compiler check,
dnl not a runtime check where additional adjustments are done in the C code.
dnl
AC_DEFUN([PHP_CHECK_AVX512_SUPPORTS],
[AC_CACHE_CHECK([whether compiler supports AVX-512],
[php_cv_have_avx512],
[save_CFLAGS="$CFLAGS"
CFLAGS="-mavx512f -mavx512cd -mavx512vl -mavx512dq -mavx512bw $CFLAGS"
AC_LINK_IFELSE([AC_LANG_SOURCE([[
  #include <immintrin.h>
  int main(void) {
    __m512i mask = _mm512_set1_epi32(0x1);
    char out[32];
    _mm512_storeu_si512(out, _mm512_shuffle_epi8(mask, mask));
    return 0;
  }]])],
  [php_cv_have_avx512=yes],
  [php_cv_have_avx512=no])
CFLAGS="$save_CFLAGS"
])
AS_VAR_IF([php_cv_have_avx512], [yes],
  [AC_DEFINE([PHP_HAVE_AVX512_SUPPORTS], [1],
    [Define to 1 if the compiler supports AVX-512.])])
])

dnl
dnl PHP_CHECK_AVX512_VBMI_SUPPORTS
dnl
dnl Check whether the compiler supports the AVX-512 extensions with the VBMI
dnl instruction set and define PHP_HAVE_AVX512_VBMI_SUPPORTS to 1 if found. Note
dnl that this is a compiler check, not a runtime check where additional
dnl adjustments are done in the C code.
dnl
AC_DEFUN([PHP_CHECK_AVX512_VBMI_SUPPORTS],
[AC_CACHE_CHECK([whether compiler supports AVX-512 VBMI],
[php_cv_have_avx512vbmi],
[save_CFLAGS="$CFLAGS"
CFLAGS="-mavx512f -mavx512cd -mavx512vl -mavx512dq -mavx512bw -mavx512vbmi $CFLAGS"
AC_LINK_IFELSE([AC_LANG_SOURCE([[
  #include <immintrin.h>
  int main(void) {
    __m512i mask = _mm512_set1_epi32(0x1);
    char out[32];
    _mm512_storeu_si512(out, _mm512_permutexvar_epi8(mask, mask));
    return 0;
  }]])],
  [php_cv_have_avx512vbmi=yes],
  [php_cv_have_avx512vbmi=no])
CFLAGS="$save_CFLAGS"
])
AS_VAR_IF([php_cv_have_avx512vbmi], [yes],
  [AC_DEFINE([PHP_HAVE_AVX512_VBMI_SUPPORTS], [1],
    [Define to 1 if the compiler supports AVX-512 VBMI.])])
])

dnl
dnl PHP_CHECK_VARIABLE_ATTRIBUTE(attribute)
dnl
dnl Check whether the compiler supports the GNU C variable attribute.
dnl
AC_DEFUN([PHP_CHECK_VARIABLE_ATTRIBUTE],
[AS_VAR_PUSHDEF([php_var], [php_cv_have_variable_attribute_$1])
AC_CACHE_CHECK([for variable __attribute__(($1))], [php_var],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([m4_case([$1],
  [aligned],
    [unsigned char test[32] __attribute__(($1(__alignof__(int))));],
  [
    m4_warn([syntax], [Unsupported variable attribute $1, the test may fail])
    int var __attribute__(($1));
  ])],
  [])],
dnl By default, compilers may not classify attribute warnings as errors
dnl (-Werror=attributes) when encountering an unknown attribute. Accept the
dnl attribute only if no warnings were generated.
  [AS_IF([test -s conftest.err],
    [AS_VAR_SET([php_var], [no])],
    [AS_VAR_SET([php_var], [yes])])],
  [AS_VAR_SET([php_var], [no])])
])
AS_VAR_IF([php_var], [yes],
  [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_ATTRIBUTE_$1]), [1],
    [Define to 1 if the compiler supports the '$1' variable attribute.])])
AS_VAR_POPDEF([php_var])
])

dnl
dnl PHP_REMOVE_OPTIMIZATION_FLAGS
dnl
dnl Removes known compiler optimization flags like -O, -O0, -O1, ..., -Ofast
dnl from CFLAGS and CXXFLAGS.
dnl
AC_DEFUN([PHP_REMOVE_OPTIMIZATION_FLAGS], [
  sed_script='s/\([[\t ]]\|^\)-O\([[0-9gsz]]\|fast\|\)\([[\t ]]\|$\)/\1/g'
  CFLAGS=$(echo "$CFLAGS" | $SED -e "$sed_script")
  CXXFLAGS=$(echo "$CXXFLAGS" | $SED -e "$sed_script")
])

dnl
dnl PHP_C_STANDARD_LIBRARY
dnl
dnl Determine the C standard library used for the build. The uclibc is checked
dnl first because it also defines the __GLIBC__ and could otherwise be detected
dnl as glibc. Musl C library is determined heuristically.
dnl
AC_DEFUN([PHP_C_STANDARD_LIBRARY],
[AC_CACHE_CHECK([C standard library implementation],
[php_cv_c_standard_library],
[php_cv_c_standard_library=unknown
dnl Check if C standard library is uclibc.
AS_VAR_IF([php_cv_c_standard_library], [unknown],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <features.h>],
[#ifndef __UCLIBC__
  (void) __UCLIBC__;
#endif])],
[php_cv_c_standard_library=uclibc],
[php_cv_c_standard_library=unknown])])
dnl Check if C standard library is GNU C.
AS_VAR_IF([php_cv_c_standard_library], [unknown],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <features.h>],
[#ifndef __GLIBC__
  (void) __GLIBC__;
#endif])],
[php_cv_c_standard_library=glibc],
[php_cv_c_standard_library=unknown])])
dnl Check if C standard library is musl libc.
AS_VAR_IF([php_cv_c_standard_library], [unknown],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <stdarg.h>],
[#ifndef __DEFINED_va_list
  (void) __DEFINED_va_list;
#endif])],
[php_cv_c_standard_library=musl],
[AS_IF([command -v ldd >/dev/null && ldd --version 2>&1 | grep ^musl >/dev/null 2>&1],
[php_cv_c_standard_library=musl],
[php_cv_c_standard_library=unknown])])])
])
])
