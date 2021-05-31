--TEST--
Resume non-running fiber
--FILE--
<?php

$fiber = new Fiber(fn() => null);

$fiber->resume();

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Fiber has not started in %sresume-non-running-fiber.php:%d
Stack trace:
#0 %sresume-non-running-fiber.php(%d): Fiber->resume()
#1 {main}
  thrown in %sresume-non-running-fiber.php on line %d
