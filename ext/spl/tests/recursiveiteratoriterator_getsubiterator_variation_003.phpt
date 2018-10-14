--TEST--
SPL: RecursiveIteratorIterator::getSubIterator() with explicit level parameter
--CREDITS--
Matt Raines matt@raines.me.uk
#testfest London 2009-05-09
--FILE--
<?php
$sample_array = array(1, 2, array(3, 4));

$iterator = new RecursiveIteratorIterator(new RecursiveArrayIterator($sample_array));

$iterator->next();
$iterator->next();
$iterator->next();
var_dump($iterator->getSubIterator(-1));
var_dump($iterator->getSubIterator(0)->getArrayCopy());
var_dump($iterator->getSubIterator(1)->getArrayCopy());
var_dump($iterator->getSubIterator(2));
?>
--EXPECT--
NULL
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
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(4)
}
NULL
