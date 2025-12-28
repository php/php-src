--TEST--
GH-16267 - overflow on socket_strerror argument
--EXTENSIONS--
sockets
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
var_dump(socket_strerror(-2147483648));
try {
	socket_strerror(2147483648);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
string(%d) "%S"
socket_strerror(): Argument #1 ($error_code) must be between %i and %d
