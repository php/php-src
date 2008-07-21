--TEST--
trigger_error() tests
--FILE--
<?php

var_dump(trigger_error());
var_dump(trigger_error("error"));
var_dump(trigger_error(array()));
var_dump(trigger_error("error", -1));
var_dump(trigger_error("error", 0));
var_dump(trigger_error("error", E_USER_WARNING));
var_dump(trigger_error("error", E_USER_DEPRECATED));

echo "Done\n";
?>
--EXPECTF--	
Warning: trigger_error() expects at least 1 parameter, 0 given in %s on line %d
NULL

Notice: error in %s on line %d
bool(true)

Warning: trigger_error() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: Invalid error type specified in %s on line %d
bool(false)

Warning: Invalid error type specified in %s on line %d
bool(false)

Warning: error in %s on line %d
bool(true)

Deprecated: error in %s on line %d
bool(true)
Done
