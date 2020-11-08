--TEST--
trigger_error() tests
--FILE--
<?php

var_dump(trigger_error("error"));

try {
    var_dump(trigger_error("error", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(trigger_error("error", 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(trigger_error("error", E_USER_WARNING));
var_dump(trigger_error("error", E_USER_DEPRECATED));

?>
--EXPECTF--
Notice: error in %s on line %d
bool(true)
trigger_error(): Argument #2 ($error_level) must be one of E_USER_ERROR, E_USER_WARNING, E_USER_NOTICE, or E_USER_DEPRECATED
trigger_error(): Argument #2 ($error_level) must be one of E_USER_ERROR, E_USER_WARNING, E_USER_NOTICE, or E_USER_DEPRECATED

Warning: error in %s on line %d
bool(true)

Deprecated: error in %s on line %d
bool(true)
