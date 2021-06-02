--TEST--
Fiber::this()
--FILE--
<?php

var_dump(Fiber::this());

$fiber = new Fiber(function (): void {
    var_dump(Fiber::this());
});

$fiber->start();

?>
--EXPECTF--
NULL
object(Fiber)#%d (0) {
}
