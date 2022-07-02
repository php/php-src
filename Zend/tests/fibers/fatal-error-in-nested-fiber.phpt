--TEST--
Fatal error within a nested fiber
--FILE--
<?php

$fiber = new Fiber(function (): void {
    $fiber = new Fiber(function (): void {
        \Fiber::suspend(2);
        trigger_error("Fatal error in nested fiber", E_USER_ERROR);
    });

    var_dump($fiber->start());

    \Fiber::suspend(1);

    $fiber->resume();
});

var_dump($fiber->start());

$fiber->resume();

?>
--EXPECTF--
int(2)
int(1)

Fatal error: Fatal error in nested fiber in %sfatal-error-in-nested-fiber.php on line %d
