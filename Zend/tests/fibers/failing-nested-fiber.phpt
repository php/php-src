--TEST--
Test throwing from fiber
--FILE--
<?php

$fiber = new Fiber(function (): void {
    $fiber = new Fiber(function (int $x, int $y): void {
        Fiber::suspend($x + $y);
        throw new Exception('test');
    });

    $value = $fiber->start(1, 2);
    var_dump($value);
    $fiber->resume($value);
});

$fiber->start();

?>
--EXPECTF--
int(3)

Fatal error: Uncaught Exception: test in %sfailing-nested-fiber.php:6
Stack trace:
#0 [internal function]: {closure:%s:%d}(1, 2)
#1 %sfailing-nested-fiber.php(%d): Fiber->resume(3)
#2 [internal function]: {closure:%s:%d}()
#3 %sfailing-nested-fiber.php(%d): Fiber->start()
#4 {main}
  thrown in %sfailing-nested-fiber.php on line %d
