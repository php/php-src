--TEST--
Clone ArrayObject using other with STD_PROP_LIST
--FILE--
<?php

$a = new ArrayObject([1, 2, 3], ArrayObject::STD_PROP_LIST);
$b = new ArrayObject($a);
$c = clone $b;
var_dump($c);

?>
--EXPECTF--
Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
