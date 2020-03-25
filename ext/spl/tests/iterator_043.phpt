--TEST--
SPL: RecursiveCachingIterator and uninitialized getChildren()
--FILE--
<?php

$it = new RecursiveCachingIterator(new RecursiveArrayIterator(array(1,2)));

var_dump($it->getChildren());
$it->rewind();
var_dump($it->getChildren());

?>
--EXPECT--
NULL
NULL
