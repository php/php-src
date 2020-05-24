--TEST--
Test nullsafe in function argument
--FILE--
<?php

class Foo {
    public $bar;
}

function set(&$ref, $value) {
    $ref = $value;
}

function test($foo) {
    try {
        set($foo?->bar, 'bar');
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
    try {
        (strrev('tes'))($foo?->bar, 'bar2');
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}

test(null);
test(new Foo());

?>
--EXPECT--
Cannot pass parameter 1 by reference
Cannot pass parameter 1 by reference
Cannot pass parameter 1 by reference
Cannot pass parameter 1 by reference
