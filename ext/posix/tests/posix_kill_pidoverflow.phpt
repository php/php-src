--TEST--
posix_kill() with large pid
--EXTENSIONS--
posix
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
// with pid overflow, it ends up being -1 which means all permissible processes are affected
try {
	posix_kill(PHP_INT_MAX, SIGTERM);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	posix_kill(PHP_INT_MIN, SIGTERM);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
posix_kill(): Argument #1 ($process_id) must be between %i and %d
posix_kill(): Argument #1 ($process_id) must be between %i and %d
