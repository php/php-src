--TEST--
Inheritance of union type from internal class
--EXTENSIONS--
zend_test
--FILE--
<?php

class C extends _ZendTestClass {}

$obj = new _ZendTestChildClass;
$obj->classUnionProp = new stdClass;
$obj->classUnionProp = new ArrayIterator;
try {
    $obj->classUnionProp = new DateTime;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$obj = new C;
$obj->classUnionProp = new stdClass;
$obj->classUnionProp = new ArrayIterator;
try {
    $obj->classUnionProp = new DateTime;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot assign DateTime to property _ZendTestClass::$classUnionProp of type stdClass|Iterator|null
Cannot assign DateTime to property _ZendTestClass::$classUnionProp of type stdClass|Iterator|null
