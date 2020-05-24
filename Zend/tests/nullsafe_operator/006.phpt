--TEST--
Test nullsafe property post increment/decrement
--FILE--
<?php

class Foo {
    public $bar = 0;
}

function test(?Foo $foo) {
    var_dump(++$foo?->bar);
    var_dump($foo?->bar);
    var_dump(--$foo?->bar);
    var_dump($foo?->bar);
}

test(null);
test(new Foo());

?>
--EXPECT--
NULL
NULL
NULL
NULL
int(1)
int(1)
int(0)
int(0)
