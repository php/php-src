--TEST--
Test that the original class name is present in the error out when extending ErrorException
--FILE--
<?php

class MyErrorException extends ErrorException{}
throw new MyErrorException(new stdClass);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: ErrorException::__construct(): Argument #1 ($message) must be of type string, DynamicObject given in %s:%d
Stack trace:
#0 %s(%d): ErrorException->__construct(Object(DynamicObject))
#1 {main}
  thrown in %sexception_020.php on line %d
