--TEST--
Testing throw exception doesn't crash with wrong params, variant 3
--FILE--
<?php

class Hello extends Error {}
throw new Hello(new stdClass);

?>
--EXPECTF--
Fatal error: Uncaught Error: Wrong parameters for Hello([string $message [, long $code [, Throwable $previous = NULL]]]) in %sexception_021.php:%d
Stack trace:
#0 %sexception_021.php(%d): Error->__construct(Object(stdClass))
#1 {main}
  thrown in %sexception_021.php on line %d
