--TEST--
Fiber::getReturn() from unstarted fiber
--FILE--
<?php

$fiber = new Fiber(fn() => Fiber::suspend(1));

$fiber->getReturn();

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot get fiber return value: The fiber has not been started in %sget-return-from-unstarted-fiber.php:%d
Stack trace:
#0 %sget-return-from-unstarted-fiber.php(%d): Fiber->getReturn()
#1 {main}
  thrown in %sget-return-from-unstarted-fiber.php on line %d
