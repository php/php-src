--TEST--
Test fetch nested nullsafe property by ref
--FILE--
<?php

class Foo {
    public $bar;
}

class Bar {
    public $baz;
}

function test(?Foo $foo) {
    var_dump($ref = &$foo?->bar->baz);
    $ref = 'baz';
    var_dump($foo?->bar->baz);
}

test(null);

$foo = new Foo();
$foo->bar = new Bar();
test($foo);

?>
--EXPECT--
NULL
NULL
NULL
string(3) "baz"
