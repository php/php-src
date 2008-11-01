--TEST--
Bug #46115 (Memory leak when calling a method using Reflection)
--SKIPIF--
<?php if (!extension_loaded('reflection')) die('skip Reflection extension not loaded'); ?>
--FILE--
<?php
$h = new RecursiveArrayIterator(array());
$x = new reflectionmethod('RecursiveArrayIterator', 'asort');
$z = $x->invoke($h);
?>
DONE
--EXPECT--
DONE
