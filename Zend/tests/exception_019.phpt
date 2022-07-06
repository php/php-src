--TEST--
Testing throw exception doesn't crash with wrong params, variant 2
--FILE--
<?php

throw new Exception(new stdClass);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Exception::__construct(): Argument #1 ($message) must be of type string, stdClass given in %s:%d
Stack trace:
#0 %sexception_019.php(%d): Exception->__construct(Object(stdClass))
#1 {main}
  thrown in %sexception_019.php on line %d
