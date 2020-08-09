--TEST--
Returned null, expected array
--FILE--
<?php
function test1() : array {
    return null;
}

test1();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: test1(): Return value must be of type array, null returned in %s:%d
Stack trace:
#0 %s(%d): test1()
#1 {main}
  thrown in %s on line %d
