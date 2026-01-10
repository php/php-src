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
set(): Argument #1 ($ref) could not be passed by reference
set(): Argument #1 ($ref) could not be passed by reference
set(): Argument #1 ($ref) could not be passed by reference
set(): Argument #1 ($ref) could not be passed by reference
