--TEST--
exception handler tests - 4
--FILE--
<?php

try {
    set_exception_handler("fo");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    set_exception_handler(array("", ""));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: set_exception_handler(): Argument #1 ($callback) must be a valid callback or null, function "fo" not found or invalid function name
TypeError: set_exception_handler(): Argument #1 ($callback) must be a valid callback or null, class "" not found
