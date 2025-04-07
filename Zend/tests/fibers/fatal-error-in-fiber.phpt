--TEST--
Fatal error in new fiber
--FILE--
<?php

$fiber = new Fiber(function (): void {
    trigger_error("Fatal error in fiber", E_USER_ERROR);
});

$fiber->start();

?>
--EXPECTF--
Deprecated: Passing E_USER_ERROR to trigger_error() is deprecated since 8.4, throw an exception or call exit with a string message instead in %s on line %d

Fatal error: Fatal error in fiber in %sfatal-error-in-fiber.php on line %d
