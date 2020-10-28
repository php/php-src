--TEST--
Exceptions cannot be cloned
--FILE--
<?php
$exception = new Exception();
$newException = clone $exception;

--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class Exception in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d