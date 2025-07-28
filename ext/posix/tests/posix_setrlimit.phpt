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
try {
	posix_setrlimit(POSIX_RLIMIT_NOFILE, 129, 128);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	posix_setrlimit(POSIX_RLIMIT_NOFILE, -2, -1);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	posix_setrlimit(POSIX_RLIMIT_NOFILE, -1, -2);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
posix_setrlimit(): Argument #2 ($soft_limit) must be lower or equal to 128
posix_setrlimit(): Argument #2 ($soft_limit) must be greater or equal to -1
posix_setrlimit(): Argument #3 ($hard_limit) must be greater or equal to -1
