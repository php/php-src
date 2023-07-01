--TEST--
trigger_error() tests
--FILE--
<?php

trigger_error("error");

try {
    trigger_error("error", -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    trigger_error("error", 0);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

trigger_error("error", E_USER_WARNING);
trigger_error("error", E_USER_DEPRECATED);

?>
--EXPECTF--
Notice: error in %s on line %d
trigger_error(): Argument #2 ($error_level) must be one of E_USER_ERROR, E_USER_WARNING, E_USER_NOTICE, or E_USER_DEPRECATED
trigger_error(): Argument #2 ($error_level) must be one of E_USER_ERROR, E_USER_WARNING, E_USER_NOTICE, or E_USER_DEPRECATED

Warning: error in %s on line %d

Deprecated: error in %s on line %d
