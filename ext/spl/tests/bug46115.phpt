--TEST--
Bug #46115 (Memory leak when calling a method using Reflection)
--FILE--
<?php
$h = new RecursiveArrayIterator(array());
$x = new reflectionmethod('RecursiveArrayIterator', 'asort');
$z = $x->invoke($h);
?>
DONE
--EXPECTF--
Deprecated: Method ArrayIterator::asort() is deprecated in %s on line %d
DONE
