--TEST--
Test ArrayObject::uasort() function : wrong arg count
--FILE--
<?php
/* Prototype  : int ArrayObject::uasort(callback cmp_function)
 * Description: proto int ArrayIterator::uasort(callback cmp_function)
 Sort the entries by values user defined function.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

$ao = new ArrayObject();

try {
	$ao->uasort();
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}

try {
	$ao->uasort(1,2);
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECT--
Function expects exactly one argument
Function expects exactly one argument
===DONE===
