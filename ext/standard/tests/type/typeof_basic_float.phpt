--TEST--
typeof() correctly recognizes float vars
--FILE--
<?php

$data = [
	-NAN,
	-INF,
	-.512E+6,
	-.512E6,
	-.5e+6,
	-.5e-6,
	-.5e6,
	-1E5,
	-1e5,
	-1.5,
	-1.0,
	-.5,
	-0.0,
	0.0,
	.5,
	1.0,
	1e5,
	1E5,
	.5e6,
	.5e+6,
	.512E6,
	.512E-6,
	.512E+6,
	+.512E-6,
	INF,
	NAN,
];

echo "\n*** typeof test: float ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum));
}

echo "\n*** typeof test extended: float ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum, true));
}

?>
--EXPECTF--

*** typeof test: float ***
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"
string(5) "float"

*** typeof test extended: float ***
string(13) "invalid float"
string(14) "infinite float"
string(14) "negative float"
string(14) "negative float"
string(14) "negative float"
string(14) "negative float"
string(14) "negative float"
string(14) "negative float"
string(14) "negative float"
string(14) "negative float"
string(14) "negative float"
string(14) "negative float"
string(5) "float"
string(5) "float"
string(14) "positive float"
string(14) "positive float"
string(14) "positive float"
string(14) "positive float"
string(14) "positive float"
string(14) "positive float"
string(14) "positive float"
string(14) "positive float"
string(14) "positive float"
string(14) "positive float"
string(14) "infinite float"
string(13) "invalid float"
