--TEST--
Delayed OOM while a call frame is only partially built (fiber VM stack)
--FILE--
<?php

function test(...$args) {}

$fiber = new Fiber(function () {
    $args = array_fill(0, 2000, 1);
    $str = str_repeat('abcdefgh,', 100);
    test(...$args, ...[explode(',', $str)]);
});
$fiber->start();

?>
===DONE===
--EXPECT--
===DONE===
