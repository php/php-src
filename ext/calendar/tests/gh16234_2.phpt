--TEST--
GH-16234 jewishtojd overflow on year argument
--EXTENSIONS--
calendar
--FILE--
<?php
try {
	jewishtojd(10, 6, PHP_INT_MIN);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
jewishtojd(10, 6, 2147483647);
echo "DONE";
?>
--EXPECTF--
jewishtojd(): Argument #3 ($year) must be between %i and %d
DONE
