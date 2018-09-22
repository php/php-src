--TEST--
Test typed properties with integer keys
--FILE--
<?php

class T {
	// Class must have at least one property. Property must have a type.
	// Empty class or untyped property removes segfault
	public int $i;
}

$t = new T;
// $x must be undefined or a non-string type
$x = 1;
$t->$x = 2;
$t->$x--;

var_dump($t);

?>
--EXPECT--
object(T)#1 (1) {
  ["i"]=>
  uninitialized(int)
  ["1"]=>
  int(1)
}
