--TEST--
Test isset and empty on nullsafe property
--FILE--
<?php

class Foo {
    public $bar;
}

class Bar {
    public $baz;
}
$bar = new Bar();
$bar->baz = 'baz';

var_dump(isset($foo?->bar));
var_dump(empty($foo?->bar));

var_dump(isset($foo?->bar->baz));
var_dump(empty($foo?->bar->baz));
echo "\n";

$foo = null;
var_dump(isset($foo?->bar));
var_dump(empty($foo?->bar));

var_dump(isset($foo?->bar->baz));
var_dump(empty($foo?->bar->baz));
echo "\n";

$foo = new Foo();
var_dump(isset($foo?->bar->baz));
var_dump(empty($foo?->bar->baz));

$foo->bar = $bar;
var_dump(isset($foo?->bar->baz));
var_dump(empty($foo?->bar->baz));

?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)

bool(false)
bool(true)
bool(false)
bool(true)

bool(false)
bool(true)
bool(true)
bool(false)
