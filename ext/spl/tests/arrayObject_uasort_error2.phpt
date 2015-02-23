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
	uasort($ao);
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}

try {
	uasort($ao, 1,2);
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
Warning: uasort() expects exactly 2 parameters, 1 given in %sarrayObject_uasort_error2.php on line %d

Warning: uasort() expects exactly 2 parameters, 3 given in %sarrayObject_uasort_error2.php on line %d
===DONE===
