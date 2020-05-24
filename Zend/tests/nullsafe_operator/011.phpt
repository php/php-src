--TEST--
Test isset and empty on nullsafe property
--FILE--
<?php

class Foo {
    public $bar;
}

var_dump(isset($foo?->bar));
var_dump(empty($foo?->bar));

$foo = null;
var_dump(isset($foo?->bar));
var_dump(empty($foo?->bar));

$foo = new Foo();
var_dump(isset($foo?->bar));
var_dump(empty($foo?->bar));

$foo->bar = 'bar';
var_dump(isset($foo?->bar));
var_dump(empty($foo?->bar));

?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
