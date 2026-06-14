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
Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
===DONE===
