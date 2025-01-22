--TEST--
GH-16501 (gmp_random_bits overflow)
--EXTENSIONS--
gmp
--FILE--
<?php
try {
	gmp_random_bits(PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
gmp_random_bits(): Argument #1 ($bits) must be between 1 and %d
