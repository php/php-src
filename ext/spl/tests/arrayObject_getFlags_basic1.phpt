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
int(0)
int(1)
int(2)
int(3)
int(3)
