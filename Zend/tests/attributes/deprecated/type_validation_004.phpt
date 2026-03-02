--TEST--
#[\Deprecated]: Type validation of $message parameter with native enum case.
--FILE--
<?php

#[\Deprecated(\Random\IntervalBoundary::ClosedOpen)]
function test() {
}
test();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Deprecated::__construct(): Argument #1 ($message) must be of type ?string, Random\IntervalBoundary given in %s:%d
Stack trace:
#0 %s(%d): Deprecated->__construct(Random\IntervalBoundary::ClosedOpen)
#1 {main}
  thrown in %s on line %d
