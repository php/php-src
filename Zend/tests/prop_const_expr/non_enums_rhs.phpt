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
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

test();
test();

?>
--EXPECT--
Error: Fetching properties on non-enums in constant expressions is not allowed
Error: Fetching properties on non-enums in constant expressions is not allowed
