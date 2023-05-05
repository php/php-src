--TEST--
Test posix_pathconf
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists("posix_pathconf")) die("skip only platforms with posix_pathconf");
?>
--FILE--
<?php
try {
	posix_pathconf('', POSIX_PC_PATH_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage(). "\n";
}
var_dump(posix_pathconf(str_repeat('non_existent', 4096), POSIX_PC_NAME_MAX));
var_dump(posix_errno() != 0);
var_dump(posix_pathconf(sys_get_temp_dir(), POSIX_PC_PATH_MAX));
?>
--EXPECTF--
posix_pathconf(): Argument #1 ($path) cannot be empty
bool(false)
bool(true)
int(%d)
