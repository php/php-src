--TEST--
GH-16878 (gmp_fact overflow)
--EXTENSIONS--
gmp
--FILE--
<?php
try {
	gmp_fact(PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	gmp_fact(gmp_init(PHP_INT_MAX));
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
gmp_fact(): Argument #1 ($num) must be between 0 and %d
gmp_fact(): Argument #1 ($num) must be between 0 and %d
