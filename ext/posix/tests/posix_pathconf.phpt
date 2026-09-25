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

/* POSIX specifically allows implementations to ignore the first
 * argument if it will not affect the result. */
$result = posix_pathconf(str_repeat('non_existent', 4096),
                         POSIX_PC_NAME_MAX);
var_dump( ($result == false && posix_errno() != 0) || is_int($result) );
var_dump(posix_pathconf(sys_get_temp_dir(), POSIX_PC_PATH_MAX));
?>
--EXPECTF--
posix_pathconf(): Argument #1 ($path) must not be empty
bool(true)
int(%d)
