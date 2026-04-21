--TEST--
Test internal class using trait with union typed property via zend_class_use_internal_traits
--EXTENSIONS--
zend_test
--FILE--
<?php

$obj = new _ZendTestClassWithUnionTypeTrait();

// Default value
var_dump($obj->unionProp);

// Assign int
$obj->unionProp = 100;
var_dump($obj->unionProp);

// Assign string
$obj->unionProp = "hello";
var_dump($obj->unionProp);

// Type error
try {
    $obj->unionProp = [];
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

// Reflection type check
$rc = new ReflectionClass(_ZendTestClassWithUnionTypeTrait::class);
$prop = $rc->getProperty('unionProp');
$type = $prop->getType();
var_dump($type instanceof ReflectionUnionType);

echo "Done\n";
?>
--EXPECTF--
int(42)
int(100)
string(5) "hello"
Cannot assign array to property _ZendTestClassWithUnionTypeTrait::$unionProp of type string|int
bool(true)
Done
