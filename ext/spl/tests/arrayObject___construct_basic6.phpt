--TEST--
SPL: ArrayObject::__construct: check impact of ArrayObject::STD_PROP_LIST on var_dump.
--FILE--
<?php
class MyArrayObject extends ArrayObject {
	private $priv1 = 'secret1';
	public $pub1 = 'public1';
} 

$ao = new ArrayObject(array(1,2,3));
$ao->p = 1;
var_dump($ao);

$ao = new ArrayObject(array(1,2,3), ArrayObject::STD_PROP_LIST);
$ao->p = 1;
var_dump($ao);

$ao = new MyArrayObject(array(1,2,3));
var_dump($ao);

$ao = new MyArrayObject(array(1,2,3), ArrayObject::STD_PROP_LIST);
var_dump($ao);
?>
--EXPECTF--
object(ArrayObject)#%d (3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
object(ArrayObject)#%d (1) {
  ["p"]=>
  int(1)
}
object(MyArrayObject)#%d (3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
object(MyArrayObject)#%d (2) {
  ["priv1:private"]=>
  string(7) "secret1"
  ["pub1"]=>
  string(7) "public1"
}