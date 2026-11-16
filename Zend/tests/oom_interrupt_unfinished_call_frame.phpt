--TEST--
Delayed OOM while a call frame is only partially built
--DESCRIPTION--
A delayed OOM throws from the VM interrupt handler, which runs between two
oplines. Argument unpacking grows test()'s frame past a VM stack page, so it is
ZEND_CALL_ALLOCATED and owns the page; the interrupt is then delivered after the
explode() call, while that frame is still pending.
--FILE--
<?php

function test(...$args) {}

$args = array_fill(0, 20000, 1);
$str = str_repeat('abcdefgh,', 300);

test(...$args, ...[explode(',', $str)]);

?>
===DONE===
--EXPECT--
===DONE===
