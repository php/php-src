--TEST--
DNF typed properties in internal classes
--EXTENSIONS--
zend_test
spl
reflection
--FILE--
<?php

$prop = new \ReflectionProperty(_ZendTestClass::class, 'dnfProperty');
$type = $prop->getType();
var_dump((string)$type);

$obj = new _ZendTestClass();
$obj->dnfProperty = new \ArrayIterator([1, 2, 3]);
try {
    $obj->dnfProperty = [];
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
string(32) "Iterator|(Traversable&Countable)"
Cannot assign array to property _ZendTestClass::$dnfProperty of type Iterator|(Traversable&Countable)

