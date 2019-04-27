--TEST--
Returned string, expected array
--FILE--
<?php
function test1() : array {
    return "hello";
}

test1();
--EXPECTF--
Fatal error: Uncaught TypeError: Return value of test1() must be of the type array, string returned in %s:%d
Stack trace:
#0 %s(%d): test1()
#1 {main}
  thrown in %s on line %d
