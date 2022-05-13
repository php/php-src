--TEST--
Replacing iterable type with intersection type
--FILE--
<?php

abstract class MyIterator implements Traversable {}

class Test {
    function method(): iterable {}
    function method2(): iterable|int {}
    function method3(): iterable|Z {}
}
class Test2 extends Test {
    function method(): X&Traversable {}
    function method2(): X&MyIterator {}
    function method3(): X&Z {}
}

?>
===DONE===
--EXPECT--
===DONE===
