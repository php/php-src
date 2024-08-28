--TEST--
GH-15613 overflow on hex strings repeater value
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64 bit platform only");
?>
--INI--
memory_limit=-1
--FILE--
<?php
try {
	unpack('h2147483647', str_repeat('X', 2**31 + 10));
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	unpack('H2147483647', str_repeat('X', 2**31 + 10));
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
unpack(): Argument #1 ($format) repeater must be less than or equal to %d
unpack(): Argument #1 ($format) repeater must be less than or equal to %d
