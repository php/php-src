--TEST--
SPL: RecursiveIteratorIterator::getSubIterator() returns different iterators depending on the current element
--CREDITS--
Matt Raines matt@raines.me.uk
#testfest London 2009-05-09
--FILE--
<?php
$sample_array = array(1, 2, array(3, 4));

$iterator = new RecursiveIteratorIterator(new RecursiveArrayIterator($sample_array));

$iterator->next();
$iterator->next();
var_dump(get_class($iterator->getSubIterator()));
var_dump($iterator->getSubIterator()->getArrayCopy());
$iterator->next();
var_dump(get_class($iterator->getSubIterator()));
var_dump($iterator->getSubIterator()->getArrayCopy());
?>
--EXPECTF--
string(22) "RecursiveArrayIterator"
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
}
string(22) "RecursiveArrayIterator"
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(4)
}
