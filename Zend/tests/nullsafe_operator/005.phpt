--TEST--
Test nullsafe property assignment op
--FILE--
<?php

class Foo {
    public $bar = 0;
}

function test(?Foo $foo) {
    var_dump($foo?->bar += 1);
    var_dump($foo?->bar);
}

test(null);
test(new Foo());

?>
--EXPECT--
NULL
NULL
int(1)
int(1)
