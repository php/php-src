--TEST--
SPL: ArrayObject::getFlags() basic usage
--FILE--
<?php
$ao = new ArrayObject(new ArrayObject(new stdClass));
var_dump($ao->getFlags());

$ao = new ArrayObject(new ArrayObject(array(1,2,3)), ArrayObject::STD_PROP_LIST);
var_dump($ao->getFlags());

$ao = new ArrayObject(new ArrayIterator(new ArrayObject()), ArrayObject::ARRAY_AS_PROPS);
var_dump($ao->getFlags());

$ao = new ArrayObject(new ArrayObject(), ArrayObject::STD_PROP_LIST|ArrayObject::ARRAY_AS_PROPS);
var_dump($ao->getFlags());

$cao = clone $ao;
var_dump($cao->getFlags());
?>
--EXPECTF--
Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
int(0)

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
int(1)

Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
int(2)

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
int(3)
int(3)
