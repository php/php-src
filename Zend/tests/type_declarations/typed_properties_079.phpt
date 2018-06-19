--TEST--
Test static typed properties with references
--FILE--
<?php

class A {
	static iterable $it = [];
	static ?array $a;
}

A::$a = &A::$it;

try {
	A::$it = new ArrayIterator();
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump(A::$it);

A::$a = &$a;

A::$it = new ArrayIterator();

try {
	$a = 1;
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump($a);

?>
--EXPECT--
string(54) "Cannot assign ArrayIterator to reference of type array"
array(0) {
}
string(45) "Cannot assign int to reference of type ?array"
NULL

