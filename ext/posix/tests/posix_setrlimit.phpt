--TEST--
posix_setrlimit(): Basic tests
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists('posix_setrlimit')) die('skip posix_setrlimit() not found');
// On FreeBSD the second call does not report an error, though the limit isn't changed either.
if (str_contains(PHP_OS, 'FreeBSD')) die('skip different behavior on FreeBSD');
?>
--FILE--
<?php

var_dump(posix_setrlimit(POSIX_RLIMIT_NOFILE, 128, 128));
var_dump(posix_setrlimit(POSIX_RLIMIT_NOFILE, 129, 128));

?>
--EXPECT--
bool(true)
bool(false)
