--TEST--
Silence operator does not leak into fiber
--FILE--
<?php

$fiber = @new Fiber(function (): void {
    trigger_error("Warning A", E_USER_WARNING);
    Fiber::suspend();
    trigger_error("Warning C", E_USER_WARNING);
});

@$fiber->start();

trigger_error("Warning B", E_USER_WARNING);

@$fiber->resume();

trigger_error("Warning D", E_USER_WARNING);

?>
--EXPECTF--
Warning: Warning A in %ssilence-operator-outside-fiber.php on line %d

Warning: Warning B in %ssilence-operator-outside-fiber.php on line %d

Warning: Warning C in %ssilence-operator-outside-fiber.php on line %d

Warning: Warning D in %ssilence-operator-outside-fiber.php on line %d
