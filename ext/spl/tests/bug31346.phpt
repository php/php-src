--TEST--
Bug #31486 (ArrayIterator::next segfaults)
--FILE--
<?php
$obj = new stdClass;
$obj->var1=1;

$ao = new ArrayObject($obj);

$i = $ao->getIterator();

$ao->offsetUnset($i->key());
$i->next();

?>
===DONE===
--EXPECTF--
===DONE===
