--TEST--
#[\NoDiscard]: Type validation of $message parameter with array.
--FILE--
<?php

#[\NoDiscard([])]
function test(): int {
	return 0;
}
test();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: NoDiscard::__construct(): Argument #1 ($message) must be of type ?string, array given in %s:%d
Stack trace:
#0 %s(%d): NoDiscard->__construct(Array)
#1 {main}
  thrown in %s on line %d
