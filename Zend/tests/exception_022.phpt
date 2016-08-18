--TEST--
Testing throw exception doesn't crash with wrong params, variant 4
--FILE--
<?php

throw new Error(new stdClass);

?>
--EXPECTF--
Fatal error: Uncaught Error: Wrong parameters for Error([string $message [, long $code [, Throwable $previous = NULL]]]) in %sexception_022.php:%d
Stack trace:
#0 %sexception_022.php(%d): Error->__construct(Object(stdClass))
#1 {main}
  thrown in %sexception_022.php on line %d
