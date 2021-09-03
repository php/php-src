--TEST--
Test nullsafe in list assign
--FILE--
<?php

class Foo {
    public $bar;
}

class Bar {
    public $baz;
}

$foo = new Foo();
$foo->bar = new Bar();

[$foo?->bar->baz] = ['bar'];
var_dump($foo);

?>
--EXPECTF--
Fatal error: Assignments can only happen to writable values in %s on line %d
