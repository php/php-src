--TEST--
SPL: ArrayObject::getFlags() - ensure flags are passed on to nested array objects and iterators.
--FILE--
<?php
$ao = new ArrayObject(array(), ArrayObject::STD_PROP_LIST|ArrayObject::ARRAY_AS_PROPS);
var_dump($ao->getFlags());

$ao2 = new ArrayObject($ao);
var_dump($ao2->getFlags());
var_dump($ao2->getIterator()->getFlags());

$ai = new ArrayIterator($ao);
var_dump($ai->getFlags());

$ao2 = new ArrayObject($ao, 0);
var_dump($ao2->getFlags());

?>
--EXPECTF--
int(3)

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
int(3)
int(3)

Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
int(3)

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
int(0)
