--TEST--
typeof() correctly recognizes null vars
--FILE--
<?php

$unset = 'some value';
unset($unset);

echo "\n*** typeof test: null ***\n";
var_dump(typeof(null));
var_dump(typeof(NULL));
var_dump(typeof($undefined_variable_1));
var_dump(typeof($unset));

echo "\n*** typeof test extended: null ***\n";
var_dump(typeof(null, true));
var_dump(typeof(NULL, true));
var_dump(typeof($undefined_variable_2, true));
var_dump(typeof($unset, true));

?>
--EXPECTF--

*** typeof test: null ***
string(4) "null"
string(4) "null"

Notice: Undefined variable: undefined_variable_1 in %s on line %d
string(4) "null"

Notice: Undefined variable: unset in %s on line %d
string(4) "null"

*** typeof test extended: null ***
string(4) "null"
string(4) "null"

Notice: Undefined variable: undefined_variable_2 in %s on line %d
string(4) "null"

Notice: Undefined variable: unset in %s on line %d
string(4) "null"
