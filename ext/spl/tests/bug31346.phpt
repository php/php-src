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
Notice: ArrayIterator::next(): Array was modified outside object and internal position is no longer valid in %sbug31346.php on line %d
===DONE===
