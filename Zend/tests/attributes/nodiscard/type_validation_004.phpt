--TEST--
#[\NoDiscard]: Type validation of $message parameter with native enum case.
--FILE--
<?php

#[\NoDiscard(\Random\IntervalBoundary::ClosedOpen)]
function test(): int {
	return 0;
}
test();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: NoDiscard::__construct(): Argument #1 ($message) must be of type ?string, Random\IntervalBoundary given in %s:%d
Stack trace:
#0 %s(%d): NoDiscard->__construct(Random\IntervalBoundary::ClosedOpen)
#1 {main}
  thrown in %s on line %d
