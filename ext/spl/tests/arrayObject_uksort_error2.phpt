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
	uksort($ao);
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}

try {
	uksort($ao, 1,2);
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
Warning: uksort() expects exactly 2 parameters, 1 given in %sarrayObject_uksort_error2.php on line %d

Warning: uksort() expects exactly 2 parameters, 3 given in %sarrayObject_uksort_error2.php on line %d
===DONE===
