--TEST--
Bug #41528 (Classes extending ArrayObject do not serialize correctly)
--FILE--
<?php
class ClassOne extends ArrayObject
{
    public $a = 2;
}

$classOne    = new ClassOne();
$classOne->a = 1;

var_dump($classOne);
var_dump($classOne->a);

$classOne = unserialize(serialize($classOne));

var_dump($classOne);
var_dump($classOne->a);
?>
--EXPECT--
object(ClassOne)#1 (2) {
  ["a"]=>
  int(1)
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
int(1)
object(ClassOne)#2 (2) {
  ["a"]=>
  int(1)
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
int(1)
