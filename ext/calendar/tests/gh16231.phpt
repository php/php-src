--TEST--
GH-16231 (jdtounix argument overflow)
--EXTENSIONS--
calendar
--FILE--
<?php
try {
	jdtounix(PHP_INT_MIN);
} catch (\ValueError $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	jdtounix(240587);
} catch (\ValueError $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
ValueError: jdtounix(): Argument #1 ($julian_day) jday must be between 2440588 and %d
ValueError: jdtounix(): Argument #1 ($julian_day) jday must be between 2440588 and %d
