--TEST--
Test nullsafe property assignment
--FILE--
<?php

class Foo {
    public $bar;
}

function test(?Foo $foo) {
    var_dump($foo?->bar = 'bar');
    var_dump($foo?->bar);
}

test(null);
test(new Foo());

?>
--EXPECT--
NULL
NULL
string(3) "bar"
string(3) "bar"
