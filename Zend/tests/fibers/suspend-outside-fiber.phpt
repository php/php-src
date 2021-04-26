--TEST--
Suspend outside fiber
--FILE--
<?php

$value = Fiber::suspend(1);

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot suspend outside of a fiber in %ssuspend-outside-fiber.php:%d
Stack trace:
#0 %ssuspend-outside-fiber.php(%d): Fiber::suspend(1)
#1 {main}
  thrown in %ssuspend-outside-fiber.php on line %d
