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

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for trigger_error() in %s on line %d
NULL

Notice: error in %s on line %d
bool(true)

Notice: Array to string conversion in %s on line %d

Notice: Array in %s on line %d
bool(true)

Warning: Invalid error type specified in %s on line %d
bool(false)

Warning: Invalid error type specified in %s on line %d
bool(false)

Warning: error in %s on line %d
bool(true)
Done
