--TEST--
Fully qualified classes are allowed in return types
--FILE--
<?php

namespace Collections;

class Foo {
    function foo(\Iterator $i): \Iterator {
        return $i;
    }
}

$foo = new Foo;
var_dump($foo->foo(new \EmptyIterator()));
?>
--EXPECTF--
object(EmptyIterator)#%d (0) {
}
