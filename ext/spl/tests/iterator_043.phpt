--TEST--
SPL: RecursiveCachingIterator and uninitialized getChildren()
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$it = new RecursiveCachingIterator(new RecursiveArrayIterator(array(1,2)));

var_dump($it->getChildren());
$it->rewind();
var_dump($it->getChildren());

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
NULL
NULL
===DONE===
