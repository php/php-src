--TEST--
Throw into non-running fiber
--FILE--
<?php

$fiber = new Fiber(fn() => null);

$fiber->throw(new Exception('test'));

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot resume a fiber that is not suspended in %sthrow-into-non-running-fiber.php:%d
Stack trace:
#0 %sthrow-into-non-running-fiber.php(%d): Fiber->throw(Object(Exception))
#1 {main}
  thrown in %sthrow-into-non-running-fiber.php on line %d
