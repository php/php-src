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
	usort($ao);
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}

try {
	usort($ao,1,2);
} catch (BadMethodCallException $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
Warning: usort() expects exactly 2 parameters, 1 given in %sarrayObject_usort_error2.php on line %d

Warning: usort() expects exactly 2 parameters, 3 given in %sarrayObject_usort_error2.php on line %d
===DONE===
