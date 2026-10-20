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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$obj = new C;
$obj->classUnionProp = new stdClass;
$obj->classUnionProp = new ArrayIterator;
try {
    $obj->classUnionProp = new DateTime;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot assign DateTime to property _ZendTestClass::$classUnionProp of type stdClass|Iterator|null
TypeError: Cannot assign DateTime to property _ZendTestClass::$classUnionProp of type stdClass|Iterator|null
