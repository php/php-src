--TEST--
Test ArrayObject::usort() function : wrong arg count
--FILE--
<?php
/* Prototype  : int ArrayObject::usort(callback cmp_function)
 * Description: proto int ArrayIterator::usort(callback cmp_function)
 Sort the entries by values user defined function.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

$ao = new ArrayObject();

try {
	$ao->usort();
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}

try {
	$ao->usort(1,2);
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
Function expects exactly one argument
Function expects exactly one argument
===DONE===
