--TEST--
exception handler tests - 4
--FILE--
<?php

try {
    set_exception_handler("fo");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    set_exception_handler(array("", ""));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
set_exception_handler(): Argument #1 ($exception_handler) must be a valid callback
set_exception_handler(): Argument #1 ($exception_handler) must be a valid callback
