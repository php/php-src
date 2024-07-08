--TEST--
GH-14873 (PHP 8.4 min function fails on typed integer)
--FILE--
<?php

function testTrim1(string $value): string {
	$value = trim($value);
	return $value;
}

function testMin2(int $value): int {
	$value = min($value, 100);
	return $value;
}

function testMin3(int $value): int {
	$value = min($value, 100, 200);
	return $value;
}

var_dump(testTrim1(" boo "));
var_dump(testMin2(5));
var_dump(testMin3(5));

?>
--EXPECT--
string(3) "boo"
int(5)
int(5)
