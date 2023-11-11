--TEST--
Error when fetching properties on non-enums in constant expressions is catchable
--FILE--
<?php

class A {
    public $prop = 42;
}

function foo($prop = (new A)->prop) {}

function test() {
    try {
        foo();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

test();
test();

?>
--EXPECT--
Fetching properties on non-enums in constant expressions is not allowed
Fetching properties on non-enums in constant expressions is not allowed
