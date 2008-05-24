--TEST--
Bug #31348 (CachingIterator::rewind() leaks)
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
