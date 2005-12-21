--TEST--
Bug #31348 (CachingIterator::rewind() leaks)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
$a = Array("some","blah");
$i = new ArrayIterator($a);

$ci = new CachingIterator($i);

$ci->rewind();

?>
===DONE===
--EXPECT--
===DONE===
