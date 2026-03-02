--TEST--
Test nullsafe operator on delayed var
--FILE--
<?php

class Foo {
    public ?Bar $bar;
}

class Bar {
    public string $baz;
}

$foo = new Foo();

$foo->bar = null;
var_dump($foo->bar?->baz);

$bar = new Bar();
$bar->baz = 'baz';
$foo->bar = $bar;
var_dump($foo->bar?->baz);

?>
--EXPECT--
NULL
string(3) "baz"
