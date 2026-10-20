--TEST--
SPL: ArrayIterator with empty string key
--FILE--
<?php

$ar = new ArrayIterator(array(''=>NULL));
var_dump($ar);
var_dump($ar->getArrayCopy());

?>
--EXPECTF--
object(ArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(1) {
    [""]=>
    NULL
  }
}
array(1) {
  [""]=>
  NULL
}
