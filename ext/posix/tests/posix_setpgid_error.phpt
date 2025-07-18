--TEST--
posix_setpgid() with wrong pid values
--EXTENSIONS--
posix
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
try {
	posix_setpgid(PHP_INT_MAX, 1);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	posix_setpgid(-2, 1);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
posix_setpgid(): Argument #1 ($process_id) must be between 0 and %d
posix_setpgid(): Argument #1 ($process_id) must be between 0 and %d
