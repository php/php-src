--TEST--
Returned 1, expected array
--FILE--
<?php
function test1() : array {
    return 1;
}
test1();
--EXPECTF--
Fatal error: Uncaught TypeError: Return value of test1() must be of the type array, int returned in %s:%d
Stack trace:
#0 %s(%d): test1()
#1 {main}
  thrown in %s on line %d
