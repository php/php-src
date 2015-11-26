--TEST--
SPL: ArrayObject
--FILE--
<?php

echo "-- Two empty iterators attached with infos that are different but same array key --\n";
$mit = new MultipleIterator(MultipleIterator::MIT_KEYS_ASSOC);
$mit ->attachIterator(new EmptyIterator(), "2");
$mit ->attachIterator(new EmptyIterator(), 2);
var_dump($mit->countIterators());
$mit->rewind();
var_dump($mit->current());

?>
--EXPECT--
-- Two empty iterators attached with infos that are different but same array key --
int(2)
array(1) {
  [2]=>
  NULL
}
