--TEST--
GH-16228 (easter_days, Overflow on year argument)
--EXTENSIONS--
calendar
--FILE--
<?php
try {
	easter_days(PHP_INT_MAX, 0);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	easter_days(-1, 0);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	easter_date(PHP_INT_MAX, 0);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
easter_days(): Argument #1 ($year) must be between 1 and %d
easter_days(): Argument #1 ($year) must be between 1 and %d
easter_date(): Argument #1 ($year) must be between 1 and %d
