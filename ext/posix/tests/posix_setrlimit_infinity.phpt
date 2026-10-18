--TEST--
posix_setrlimit(): accepts POSIX_RLIMIT_INFINITY
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists('posix_setrlimit')) die('skip posix_setrlimit() not found');
?>
--FILE--
<?php
var_dump(is_bool(posix_setrlimit(POSIX_RLIMIT_FSIZE, POSIX_RLIMIT_INFINITY, POSIX_RLIMIT_INFINITY)));
?>
--EXPECT--
bool(true)
