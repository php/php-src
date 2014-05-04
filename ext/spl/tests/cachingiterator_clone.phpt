--TEST--
SPL: CachingIterator clone tests
--FILE--
<?php
$arrayIterator = new ArrayIterator(array(1,2,3));
$it1 = new CachingIterator($arrayIterator);
$it1->rewind();
$it2=clone $it1;
$it2->next();
var_dump($it1->current());
$it1->next();
var_dump($it1->current());
?>
--EXPECT--
int(1)
int(2)
