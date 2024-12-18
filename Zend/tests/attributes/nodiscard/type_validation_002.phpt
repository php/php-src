--TEST--
#[\NoDiscard]: Type validation of $message parameter with int and strict types.
--FILE--
<?php
declare(strict_types = 1);

#[\NoDiscard(1234)]
function test(): int {
	return 0;
}

test();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: NoDiscard::__construct(): Argument #1 ($message) must be of type ?string, int given in %s:%d
Stack trace:
#0 %s(%d): NoDiscard->__construct(1234)
#1 {main}
  thrown in %s on line %d
