--TEST--
GH-16320 mb_substr overflow from negative length
--EXTENSIONS--
mbstring
--FILE--
<?php
try {
	mb_substr("abcd", PHP_INT_MIN, 4, "UTF-8");
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	mb_substr("abcd", 0, PHP_INT_MIN, "UTF-8");
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
var_dump(mb_substr("abcd", PHP_INT_MAX, PHP_INT_MAX, "UTF-8"));
?>
--EXPECTF--
mb_substr(): Argument #2 ($start) must be between %s and %s
mb_substr(): Argument #3 ($length) must be between %s and %s
string(0) ""

