--TEST--
input_has_variable()
--GET--
a=qwe&abc=<a>href</a>
--POST--
b=qwe&bbc=<a>href</a>
--FILE--
<?php

var_dump(input_has_variable(INPUT_GET, "a"));
var_dump(input_has_variable(INPUT_GET, "abc"));
var_dump(input_has_variable(INPUT_GET, "nonex"));
var_dump(input_has_variable(INPUT_GET, " "));
var_dump(input_has_variable(INPUT_GET, ""));
var_dump(input_has_variable(INPUT_GET, array()));

var_dump(input_has_variable(INPUT_POST, "b"));
var_dump(input_has_variable(INPUT_POST, "bbc"));
var_dump(input_has_variable(INPUT_POST, "nonex"));
var_dump(input_has_variable(INPUT_POST, " "));
var_dump(input_has_variable(INPUT_POST, ""));
var_dump(input_has_variable(INPUT_POST, array()));

var_dump(input_has_variable(-1, ""));
var_dump(input_has_variable("", ""));
var_dump(input_has_variable(array(), array()));
var_dump(input_has_variable(array(), ""));
var_dump(input_has_variable("", array()));

echo "Done\n";
?>
--EXPECTF--	
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)

Warning: input_has_variable() expects parameter 2 to be string, array given in %s on line %d
NULL
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)

Warning: input_has_variable() expects parameter 2 to be string, array given in %s on line %d
NULL
bool(false)

Warning: input_has_variable() expects parameter 1 to be long, string given in %s on line %d
NULL

Warning: input_has_variable() expects parameter 1 to be long, array given in %s on line %d
NULL

Warning: input_has_variable() expects parameter 1 to be long, array given in %s on line %d
NULL

Warning: input_has_variable() expects parameter 1 to be long, string given in %s on line %d
NULL
Done
