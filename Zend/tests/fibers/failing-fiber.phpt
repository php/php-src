--TEST--
Test throwing from fiber
--FILE--
<?php

$fiber = new Fiber(function (): void {
    Fiber::suspend('test');
    throw new Exception('test');
});

$value = $fiber->start();
var_dump($value);

$fiber->resume($value);

?>
--EXPECTF--
string(4) "test"

Fatal error: Uncaught Exception: test in %sfailing-fiber.php:%d
Stack trace:
#0 [internal function]: {closure}()
#1 %sfailing-fiber.php(%d): Fiber->resume('test')
#2 {main}
  thrown in %sfailing-fiber.php on line %d
