--TEST--
Test ArrayObject::uksort() function : wrong arg count
--FILE--
<?php
/* Prototype  : int ArrayObject::uksort(callback cmp_function)
 * Description: proto int ArrayIterator::uksort(callback cmp_function)
 Sort the entries by key using user defined function.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

$ao = new ArrayObject();

try {
	$ao->uksort();
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}

try {
	$ao->uksort(1,2);
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
Function expects exactly one argument
Function expects exactly one argument
===DONE===
