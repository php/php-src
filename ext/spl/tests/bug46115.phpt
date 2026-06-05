--TEST--
Bug #46115 (Memory leak when calling a method using Reflection)
--FILE--
<?php
$h = new RecursiveArrayIterator(array());
$x = new ReflectionMethod('RecursiveArrayIterator', 'asort');
$z = $x->invoke($h);
?>
DONE
--EXPECT--
DONE
