--TEST--
Replacing object type with intersection type
--FILE--
<?php

// It's sufficient that Y is loadable.
interface Y {}

class Test {
    function method(): object {}
    function method2(): object|int {}
    function method3(): Y|int {}
}
class Test2 extends Test {
    function method(): X&Y {}
    function method2(): X&Y {}
    function method3(): X&Y {}
}

?>
===DONE===
--EXPECT--
===DONE===
