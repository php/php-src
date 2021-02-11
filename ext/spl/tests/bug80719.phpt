--TEST--
Bug #80719: Iterating after failed ArrayObject::setIteratorClass() causes Segmentation fault
--FILE--
<?php

$array = new ArrayObject([42]);
try {
    $array->setIteratorClass(FilterIterator::class);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
foreach ($array as $v) {
    var_dump($v);
}

?>
--EXPECT--
ArrayObject::setIteratorClass(): Argument #1 ($iteratorClass) must be a class name derived from ArrayIterator, FilterIterator given
int(42)
