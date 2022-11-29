--TEST--
preload_user sets the process uid
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_user.inc
opcache.preload_user={ENV:TEST_NON_ROOT_USER}
opcache.log_verbosity_level=2
--EXTENSIONS--
opcache
posix
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (posix_geteuid() !== 0) die('skip Test needs root user');
?>
--FILE--
<?php
var_dump(function_exists("f1"));
var_dump(function_exists("f2"));
?>
OK
--EXPECTF--
bool(false)
bool(true)
bool(false)
OK
