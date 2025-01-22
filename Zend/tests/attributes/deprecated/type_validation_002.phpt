--TEST--
#[\Deprecated]: Type validation of $message parameter with int and strict types.
--FILE--
<?php
declare(strict_types = 1);

#[\Deprecated(1234)]
function test() {
}

test();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Deprecated::__construct(): Argument #1 ($message) must be of type ?string, int given in %s:%d
Stack trace:
#0 %s(%d): Deprecated->__construct(1234)
#1 {main}
  thrown in %s on line %d
