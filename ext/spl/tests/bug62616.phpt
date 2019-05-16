--TEST--
Bug #62616 (ArrayIterator::count() from IteratorIterator instance gives Segmentation fault)
--FILE--
<?php
$ai = new ArrayIterator(array(0,1));

var_dump($ai->count());

$ii = new IteratorIterator($ai);

var_dump($ii->count());
?>
--EXPECT--
int(2)
int(2)
