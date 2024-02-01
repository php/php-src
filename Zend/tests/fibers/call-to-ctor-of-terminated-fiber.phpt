--TEST--
Multiple calls to constructor are prevented after fiber terminated
--FILE--
<?php

$fiber = new Fiber(function () {
    return 123;
});

var_dump($fiber->start());
var_dump($fiber->getReturn());

$fiber->__construct(function () {
    return 321;
});

?>
--EXPECTF--
NULL
int(123)

Fatal error: Uncaught FiberError: Cannot call constructor twice in %scall-to-ctor-of-terminated-fiber.php:%d
Stack trace:
#0 %scall-to-ctor-of-terminated-fiber.php(%d): Fiber->__construct(Object(Closure))
#1 {main}
  thrown in %scall-to-ctor-of-terminated-fiber.php on line %d
