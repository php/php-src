--TEST--
#[\Deprecated]: Type validation of $message parameter with array.
--FILE--
<?php

#[\Deprecated([])]
function test() {
}
test();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Deprecated::__construct(): Argument #1 ($message) must be of type ?string, array given in %s:%d
Stack trace:
#0 %s(%d): Deprecated->__construct(Array)
#1 {main}
  thrown in %s on line %d
