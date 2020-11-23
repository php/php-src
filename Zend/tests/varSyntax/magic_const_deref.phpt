--TEST--
Dereferencing of magic constants
--FILE--
<?php

function test() {
    var_dump(__FUNCTION__[0]);
    var_dump(__FUNCTION__->prop);
    try {
        __FUNCTION__->method();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

test();

?>
--EXPECTF--
string(1) "t"

Warning: Attempt to read property "prop" on string in %s on line %d
NULL
Call to a member function method() on string
