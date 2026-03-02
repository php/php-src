--TEST--
pcntl_sigwaitinfo abort when signals is an array with self-reference.
--EXTENSIONS--
pcntl
--SKIPIF--
<?php if (!function_exists("pcntl_sigwaitinfo")) die("skip pcntl_sigwaitinfo() not available"); ?>
--FILE--
<?php
$a[0] = &$a;

try {
	pcntl_sigwaitinfo($a,$a);
} catch(\TypeError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
pcntl_sigwaitinfo(): Argument #1 ($signals) signals must be of type int, array given
