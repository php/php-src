--TEST--
Replacing object type with intersection type
--FILE--
<?php

// It's sufficient that Y is loadable.
interface Y {}

class Test {
    function method1(): object {}
    function method2(): object|int {}
    function method3(): Y|int {}
}
class Test2 extends Test {
    function method1(): (X&Y)|Countable {}
    function method2(): (X&Y)|int {}
    function method3(): (X&Y)|int {}
}

?>
===DONE===
--EXPECT--
===DONE===
