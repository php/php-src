--TEST--
GH-14774 time_sleep_until overflow
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
foreach([INF, -INF, 10e300, -10e300, NAN, -NAN] as $var) {
	try {
		time_sleep_until($var);
	} catch (\ValueError $e) {
		echo $e->getMessage() . PHP_EOL;
	}
}
?>
--EXPECTF--
time_sleep_until(): Argument #1 ($timestamp) must be between 0 and %d
time_sleep_until(): Argument #1 ($timestamp) must be between 0 and %d
time_sleep_until(): Argument #1 ($timestamp) must be between 0 and %d
time_sleep_until(): Argument #1 ($timestamp) must be between 0 and %d
time_sleep_until(): Argument #1 ($timestamp) must be between 0 and %d
time_sleep_until(): Argument #1 ($timestamp) must be between 0 and %d
