--TEST--
GH-10437 (Segfault/assertion when using fibers in shutdown function after bailout)
--FILE--
<?php

register_shutdown_function(function (): void {
    var_dump(Fiber::getCurrent());
});

$fiber = new Fiber(function (): never {
    trigger_error('Bailout in fiber', E_USER_ERROR);
});
$fiber->start();

?>
--EXPECTF--
Fatal error: Bailout in fiber in %sgh10437.php on line %d
NULL
