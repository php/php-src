--TEST--
SPL: ArrayObject::__construct: Using object with shared properties
--FILE--
<?php
$y = 2;
$x = 1;
$a = array($y, $x);
$o = (object)$a;
$ao = new ArrayObject($o);
$ao->asort();
var_dump($a, $o, $ao);
?>
--EXPECT--
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
object(stdClass)#1 (2) {
  [1]=>
  int(1)
  [0]=>
  int(2)
}
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(stdClass)#1 (2) {
    [1]=>
    int(1)
    [0]=>
    int(2)
  }
}
