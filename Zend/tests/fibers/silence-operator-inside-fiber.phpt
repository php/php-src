--TEST--
Silence operator does not leak out of fiber
--FILE--
<?php

function suspend_with_warnings(): void {
    trigger_error("Warning A", E_USER_WARNING); // Should be silenced.
    Fiber::suspend();
    trigger_error("Warning B", E_USER_WARNING); // Should be silenced.
}

$fiber = new Fiber(function (): void {
    @suspend_with_warnings();
});

$fiber->start();

trigger_error("Warning C", E_USER_WARNING);

$fiber->resume();

trigger_error("Warning D", E_USER_WARNING);

?>
--EXPECTF--
Warning: Warning C in %ssilence-operator-inside-fiber.php on line %d

Warning: Warning D in %ssilence-operator-inside-fiber.php on line %d
