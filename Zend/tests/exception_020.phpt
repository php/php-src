--TEST--
Test that the original class name is present in the error out when extending ErrorException
--FILE--
<?php

class MyErrorException extends ErrorException{}
throw new MyErrorException(new stdClass);

?>
--EXPECTF--
Fatal error: Uncaught Error: Wrong parameters for MyErrorException([string $message [, long $code, [ long $severity, [ string $filename, [ long $lineno  [, Throwable $previous = NULL]]]]]]) in %sexception_020.php:%d
Stack trace:
#0 %sexception_020.php(%d): ErrorException->__construct(Object(stdClass))
#1 {main}
  thrown in %sexception_020.php on line %d
