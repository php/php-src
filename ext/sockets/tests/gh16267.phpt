--TEST--
GH-16267 - overflow on socket_strerror argument
--EXTENSIONS--
sockets
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die('skip 64-bit only'); ?>
--FILE--
<?php
try {
	socket_strerror(PHP_INT_MIN);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	socket_strerror(PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
socket_strerror(): Argument #1 ($error_code) must be between %s and %s
socket_strerror(): Argument #1 ($error_code) must be between %s and %s
