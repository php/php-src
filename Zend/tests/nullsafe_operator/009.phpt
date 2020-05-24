--TEST--
Test fetch nullsafe property by ref
--FILE--
<?php

class Foo {
    public $bar;
}

function test(?Foo $foo) {
    var_dump($ref = &$foo?->bar);
    $ref = 'bar';
    var_dump($foo?->bar);
}

test(null);
test(new Foo());

?>
--EXPECT--
NULL
NULL
NULL
string(3) "bar"
