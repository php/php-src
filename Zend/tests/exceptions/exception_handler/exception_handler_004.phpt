--TEST--
exception handler tests - 4
--FILE--
<?php

try {
    set_exception_handler("fo");
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    set_exception_handler(array("", ""));
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: set_exception_handler(): Argument #1 ($callback) must be a valid callback or null, function "fo" not found or invalid function name
TypeError: set_exception_handler(): Argument #1 ($callback) must be a valid callback or null, class "" not found
