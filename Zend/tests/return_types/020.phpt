--TEST--
Exception thrown from function with return type

--FILE--
<?php
function test() : array {
    throw new Exception();
}

test();

--EXPECTF--
Exception: (empty message) in %s on line %d
Stack trace:
#0 %s(%d): test()
#1 {main}
