--TEST--
preload_user has no effect when euid is not 0
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
opcache.preload_user=php
opcache.log_verbosity_level=2
--EXTENSIONS--
opcache
posix
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (posix_geteuid() === 0) die('skip Test needs non-root user');
?>
--FILE--
<?php
var_dump(function_exists("f1"));
var_dump(function_exists("f2"));
?>
OK
--EXPECTF--
%sWarning "opcache.preload_user" is ignored because the current user is not "root"
bool(true)
bool(false)
OK
