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
--EXPECTF--
Deprecated: Creation of dynamic property T::$1 is deprecated in %s on line %d
object(T)#1 (1) {
  ["i"]=>
  uninitialized(int)
  ["1"]=>
  int(1)
}
