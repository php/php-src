--TEST--
Fiber function may return by ref, but getReturn() always returns by val
--FILE--
<?php

$fiber = new Fiber(function &() {
    Fiber::suspend();
    return $var;
});

$fiber->start();
$fiber->resume();
var_dump($fiber->getReturn());

?>
--EXPECT--
NULL
