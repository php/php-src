#!/bin/sh

# first find which PHP binary to use
if test "x$PHP_PEAR_PHP_BIN" != "x"; then
  PHP="$PHP_PEAR_PHP_BIN"
else
  if test "@php_bin@" = '@'php_bin'@'; then
    PHP=php 
  else
    PHP="@php_bin@"
  fi
fi

# then look for the right pear include dir
if test "x$PHP_PEAR_INSTALL_DIR" != "x"; then
  INCDIR=$PHP_PEAR_INSTALL_DIR
  INCARG="-d include_path=$PHP_PEAR_INSTALL_DIR"
else
  if test "@php_dir@" = '@'php_dir'@'; then
    INCDIR=`dirname $0`
    INCARG=""  
  else
    INCDIR="@php_dir@"
    INCARG="-d include_path=@php_dir@"
  fi
fi

exec $PHP -C -q $INCARG -d output_buffering=1 $INCDIR/pearcmd.php $@
