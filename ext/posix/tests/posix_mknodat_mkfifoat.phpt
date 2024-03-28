--TEST--
posix_mknodat/posix_mkfifoat support
--EXTENSIONS--
posix
--SKIPIF--
<?php
// usually, mkfifoat is implemented with mknodat
if (!function_exists('posix_mknodat') || !function_exists('posix_mkfifoat')) die('skip posix_mknodat()/posix_mkfifoat not found');
if (getenv('TRAVIS')) die('skip Currently fails on Travis');
?>
--FILE--
<?php

$fd = fopen(__DIR__, "r");
var_dump(posix_mknodat($fd, 'dev', POSIX_S_IFIFO | 0666, 1, 0));
var_dump(posix_mknodat($fd, '', POSIX_S_IFBLK | 0777, 1, 0));
var_dump(posix_mknodat($fd, __DIR__ . '', POSIX_S_IFBLK | 0777, 1, 0));
try {
	posix_mknodat($fd, __DIR__ . '/dev/', POSIX_S_IFBLK | 0777, 0, 0);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
var_dump(posix_mkfifoat($fd, '', POSIX_S_IFBLK | 0777));
var_dump(posix_mkfifoat($fd, __DIR__ . '', POSIX_S_IFBLK | 0777));
try {
	posix_mkfifoat(new stdClass(), '/dev/', POSIX_S_IFBLK | 0777);
} catch (TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
@unlink(__DIR__ . '/dev');
var_dump(posix_mkfifoat($fd, 'dev', POSIX_S_IFIFO | 0666));
fclose($fd);
@unlink(__DIR__ . '/dev');
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
posix_mknodat(): Argument #4 ($major) cannot be 0 for the POSIX_S_IFCHR and POSIX_S_IFBLK modes
bool(false)
bool(false)
posix_mkfifoat(): Argument #1 ($file_descriptor) must be of type int|resource, stdClass given
bool(true)

