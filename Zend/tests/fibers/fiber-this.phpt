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
object(Fiber)#%d (0) {
}
object(Fiber)#%d (0) {
}
