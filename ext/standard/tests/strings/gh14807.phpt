--TEST--
GH-14807 overflow on insertion_cost/replacement_cost/deletion_cost
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
$str1 = "abcd";
$str2 = "defg";

foreach([PHP_INT_MIN, PHP_INT_MAX] as $lim) {
	try {
		levenshtein($str1, $str2, $lim);
	} catch (\ValueError $e) {
		echo $e->getMessage() . PHP_EOL;
	}

	try {
		levenshtein($str1, $str2, 1, $lim);
	} catch (\ValueError $e) {
		echo $e->getMessage() . PHP_EOL;
	}

	try {
		levenshtein($str1, $str2, 1, 1, $lim);
	} catch (\ValueError $e) {
		echo $e->getMessage() . PHP_EOL;
	}
}
?>
--EXPECTF--
levenshtein(): Argument #3 ($insertion_cost) must be at least %s
levenshtein(): Argument #4 ($replacement_cost) must be at least %s
levenshtein(): Argument #5 ($deletion_cost) must be at least %s
levenshtein(): Argument #3 ($insertion_cost) must be at most %s
levenshtein(): Argument #4 ($replacement_cost) must be at most %s
levenshtein(): Argument #5 ($deletion_cost) must be at most %s
