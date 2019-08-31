--TEST--
trigger_error() tests
--FILE--
<?php

var_dump(trigger_error("error"));
var_dump(trigger_error("error", -1));
var_dump(trigger_error("error", 0));
var_dump(trigger_error("error", E_USER_WARNING));
var_dump(trigger_error("error", E_USER_DEPRECATED));

echo "Done\n";
?>
--EXPECTF--
Notice: error in %s on line %d
bool(true)

Warning: Invalid error type specified in %s on line %d
bool(false)

Warning: Invalid error type specified in %s on line %d
bool(false)

Warning: error in %s on line %d
bool(true)

Deprecated: error in %s on line %d
bool(true)
Done
