--TEST--
Resume terminated fiber
--FILE--
<?php

$fiber = new Fiber(fn() => null);

$fiber->start();

$fiber->resume();

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot resume a fiber that is not suspended in %sresume-terminated-fiber.php:%d
Stack trace:
#0 %sresume-terminated-fiber.php(%d): Fiber->resume()
#1 {main}
  thrown in %sresume-terminated-fiber.php on line %d
