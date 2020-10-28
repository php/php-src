--TEST--
Errors cannot be cloned
--FILE--
<?php
$error = new Error();
$newError = clone $error;

--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class Error in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d