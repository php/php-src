--TEST--
Test var_dump() function function with circular objects
--FILE--
<?php

class Circular {
    public object $obj;
}

$recursion_obj1 = new Circular();
$recursion_obj2 = new Circular();
$recursion_obj1->obj = $recursion_obj2;
$recursion_obj2->obj = $recursion_obj1;
var_dump($recursion_obj2);

?>
--EXPECT--
object(Circular)#2 (1) {
  ["obj"]=>
  object(Circular)#1 (1) {
    ["obj"]=>
    *RECURSION*
  }
}
