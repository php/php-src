--TEST--
Bug #67360 (Missing element after ArrayObject::getIterator)
--FILE--
<?php

$array = array('' => 1, 1 => 2, 3 => 4);
$ArrayObject = new ArrayObject($array);
var_dump($ArrayObject);
$Iterator = $ArrayObject->getIterator();
var_dump(count($Iterator) === count($array));
var_dump(iterator_to_array($Iterator));

?>
--EXPECTF--
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [""]=>
    int(1)
    [1]=>
    int(2)
    [3]=>
    int(4)
  }
}
bool(true)
array(3) {
  [""]=>
  int(1)
  [1]=>
  int(2)
  [3]=>
  int(4)
}
