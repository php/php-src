--TEST--
Fiber::getCurrent()
--FILE--
<?php

var_dump(Fiber::getCurrent());

$fiber = new Fiber(function (): void {
    var_dump(Fiber::getCurrent());
});

$fiber->start();

?>
--EXPECTF--
NULL
object(Fiber)#%d (0) {
}
