--TEST--
Clone ArrayObject using other with STD_PROP_LIST
--FILE--
<?php

$a = new ArrayObject([1, 2, 3], ArrayObject::STD_PROP_LIST);
$b = new ArrayObject($a);
$c = clone $b;
var_dump($c);

?>
--EXPECT--
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
