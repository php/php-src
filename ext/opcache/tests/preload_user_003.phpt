--TEST--
preload_user has no effect when preload_user is the current user
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_user.inc
opcache.preload_user=root
opcache.log_verbosity_level=2
--EXTENSIONS--
opcache
posix
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (posix_geteuid() !== 0) die('skip Test needs root user');
if (posix_getpwnam('root') === false) die('skip Root user does not exist');
?>
--FILE--
<?php
var_dump(function_exists("f1"));
var_dump(function_exists("f2"));
?>
OK
--EXPECTF--
bool(true)
bool(true)
bool(false)
OK
