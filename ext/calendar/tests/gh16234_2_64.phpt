--TEST--
GH-16234 jewishtojd overflow on year argument
--EXTENSIONS--
calendar
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) {
        die("skip this test is for 64bit platform only");
}
?>
--FILE--
<?php
try {
	jewishtojd(10, 6, PHP_INT_MIN);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
jewishtojd(): Argument #3 ($year) must be between %i and %d

