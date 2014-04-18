--TEST--
Fully qualified classes in trait return types

--FILE--
<?php

namespace FooSpace;

use ArrayIterator;

trait Fooable {
    function foo(array $data): \Iterator {
        return new ArrayIterator($data);
    }
}

class Foo {

    use Fooable;


}

$foo = new Foo;
var_dump($foo->foo([]));


--EXPECTF--
object(ArrayIterator)#%d (%d) {
  ["storage":"ArrayIterator":private]=>
  array(0) {
  }
}
