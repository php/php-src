--TEST--
SPL: CallbackFilterIterator clone tests
--FILE--
<?php
$filter_callback = function ($e) {
  return true;
};

$arrayIterator = new ArrayIterator(array(1,2,3));
$it1 = new CallbackFilterIterator($arrayIterator, $filter_callback);
$it1->rewind();
$it2 = clone $it1;
$it2->next();
var_dump($it1->current());
?>
--EXPECT--
int(1)
