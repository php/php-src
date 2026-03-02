--TEST--
GH-16231 (jdtounix argument overflow)
--EXTENSIONS--
calendar
--FILE--
<?php
try {
	jdtounix(PHP_INT_MIN);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	jdtounix(240587);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
jday must be between 2440588 and %d
jday must be between 2440588 and %d
