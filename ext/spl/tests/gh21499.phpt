--TEST--
GH-21499: RecursiveArrayIterator getChildren UAF after parent free
--ENV--
USE_ZEND_ALLOC=0
--FILE--
<?php
$it = new RecursiveArrayIterator([[1]]);
$child = $it->getChildren();
unset($it);
$child->__construct([2, 3]);
var_dump($child->getArrayCopy());
?>
--EXPECT--
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
