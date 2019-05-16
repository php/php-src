--TEST--
Fully qualified classes in trait return types
--FILE--
<?php

namespace FooSpace;

trait Fooable {
    function foo(): \Iterator {
        return new \EmptyIterator();
    }
}

class Foo {
    use Fooable;
}

$foo = new Foo;
var_dump($foo->foo([]));
--EXPECTF--
object(EmptyIterator)#%d (%d) {
}
