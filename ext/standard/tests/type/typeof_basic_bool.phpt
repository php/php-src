--TEST--
typeof() correctly recognizes bool vars
--FILE--
<?php

$data = [
	false,
	FALSE,
	true,
	TRUE,
];

echo "\n*** typeof test: bool ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum));
}

echo "\n*** typeof test extended: bool ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum, true));
}

?>
--EXPECTF--

*** typeof test: bool ***
string(4) "bool"
string(4) "bool"
string(4) "bool"
string(4) "bool"

*** typeof test extended: bool ***
string(10) "bool false"
string(10) "bool false"
string(9) "bool true"
string(9) "bool true"
