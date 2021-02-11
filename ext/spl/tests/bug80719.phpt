--TEST--
Bug #80719: Iterating after failed ArrayObject::setIteratorClass() causes Segmentation fault
--FILE--
<?php

$array = new ArrayObject([42]);
$array->setIteratorClass(FilterIterator::class);
foreach ($array as $v) {
    var_dump($v);
}

?>
--EXPECTF--
Warning: ArrayObject::setIteratorClass() expects parameter 1 to be a class name derived from ArrayIterator, 'FilterIterator' given in %s on line %d
int(42)
