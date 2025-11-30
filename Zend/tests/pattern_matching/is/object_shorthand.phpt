--TEST--
Object pattern matching
--FILE--
<?php

class Foo {
    public function __construct(public $a, public $b) {}
}

var_dump(new Foo(1, 2) is Foo(:$a, b: 2));
var_dump($a);
var_dump(new Foo(1, 2) is Foo(a: 3, :$b));
var_dump($b);

?>
--EXPECTF--
bool(true)
int(1)
bool(false)

Warning: Undefined variable $b in %s on line %d
NULL
