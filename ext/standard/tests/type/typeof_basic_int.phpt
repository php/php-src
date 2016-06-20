--TEST--
typeof() correctly recognizes int vars
--FILE--
<?php

$data = [
	PHP_INT_MIN,
	-001,
	-0x1,
	-0b1,
	-1,
	-0,
	0,
	1,
	0b1,
	0x1,
	001,
	PHP_INT_MAX,
];

echo "\n*** typeof test: int ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum));
}

echo "\n*** typeof test extended: int ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum, true));
}

?>
--EXPECTF--

*** typeof test: int ***
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"

*** typeof test extended: int ***
string(12) "negative int"
string(12) "negative int"
string(12) "negative int"
string(12) "negative int"
string(12) "negative int"
string(8) "zero int"
string(8) "zero int"
string(12) "positive int"
string(12) "positive int"
string(12) "positive int"
string(12) "positive int"
string(12) "positive int"
