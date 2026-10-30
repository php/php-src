--TEST--
Test posix_fpathconf
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists("posix_pathconf")) die("skip only platforms with posix_pathconf");
?>
--FILE--
<?php
var_dump(posix_fpathconf(-1, POSIX_PC_PATH_MAX));
var_dump(posix_errno() != 0);
try {
	posix_fpathconf("string arg", POSIX_PC_PATH_MAX);
} catch (\TypeError $e) {
	echo $e->getMessage() . "\n";
}
$fd = fopen(__DIR__, "r");
var_dump(posix_fpathconf($fd, POSIX_PC_PATH_MAX));
fclose($fd);
?>
--EXPECTF--
bool(false)
bool(true)
posix_fpathconf(): Argument #1 ($file_descriptor) must be of type int|resource, string given
int(%d)
