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

Deprecated: Passing E_USER_ERROR to trigger_error() is deprecated since 8.4, throw an exception or call exit with a string message instead in %s on line %d

Fatal error: Fatal error in nested fiber in %sfatal-error-in-nested-fiber.php on line %d
