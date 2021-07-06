--TEST--
Resume previous fiber
--FILE--
<?php

$fiber = new Fiber(function (): void {
    $fiber1 = Fiber::getCurrent();

    $fiber2 = new Fiber(function () use ($fiber1): void {
        $fiber1->resume();
    });

    $fiber2->start();
});

$fiber->start();

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot resume a fiber that is not suspended in %sresume-previous-fiber.php:%d
Stack trace:
#0 %sresume-previous-fiber.php(%d): Fiber->resume()
#1 [internal function]: {closure}()
#2 %sresume-previous-fiber.php(%d): Fiber->start()
#3 [internal function]: {closure}()
#4 %sresume-previous-fiber.php(%d): Fiber->start()
#5 {main}
  thrown in %sresume-previous-fiber.php on line %d
