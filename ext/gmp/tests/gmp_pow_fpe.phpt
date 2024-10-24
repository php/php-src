--TEST--
gmp_pow() floating point exception
--EXTENSIONS--
gmp
--FILE--
<?php
$g = gmp_init(256);

try {
	gmp_pow($g, PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	gmp_pow(256, PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
    gmp_pow(gmp_add(gmp_mul(gmp_init(PHP_INT_MAX), gmp_init(PHP_INT_MAX)), 3), 256);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
    gmp_pow(gmp_init(PHP_INT_MAX), 256);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
base and exponent overflow
base and exponent overflow
base and exponent overflow
base and exponent overflow
